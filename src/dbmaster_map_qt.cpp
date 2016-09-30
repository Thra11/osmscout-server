#ifdef USE_OSMSCOUT_MAP_QT

#include "dbmaster.h"
#include "appsettings.h"
#include "config.h"

#include <osmscout/MapPainterQt.h>

#include <QMutexLocker>
#include <QPixmap>
#include <QImage>
#include <QPainter>
#include <QBuffer>

bool DBMaster::renderMap(bool daylight, double dpi, int zoom_level, int width, int height, double lat, double lon, QByteArray &result)
{
    if (m_error_flag) return false;

    // reading in settings - area protected by mutex
    m_mutex.lock();

    bool renderSea = m_render_sea;
    bool drawBackground = m_draw_background;
    float fontSize = m_font_size;

    std::list<std::string> paths;
    paths.push_back(m_icons_dir);

    if ( !loadStyle(daylight) )
        return false;

    m_mutex.unlock();
    // settings read in - mutex protected area ends

    osmscout::MercatorProjection  projection;
    osmscout::MapParameter        drawParameter;
    osmscout::AreaSearchParameter searchParameter;
    osmscout::MapData             data;

    osmscout::Magnification magnification;
    magnification.SetLevel(zoom_level);

    drawParameter.SetIconPaths(paths);
    //drawParameter.SetPatternPaths(paths);

    drawParameter.SetDebugData(false);
    drawParameter.SetDebugPerformance(false);

    drawParameter.SetFontSize(fontSize);

    // optimize process can reduce number of nodes before rendering
    // it helps for slow renderer backend, but it cost some cpu
    // it seems that it is better to disable it for mobile devices with slow cpu
    drawParameter.SetOptimizeWayNodes(osmscout::TransPolygon::none);
    drawParameter.SetOptimizeAreaNodes(osmscout::TransPolygon::none);

    drawParameter.SetRenderBackground(drawBackground || renderSea);
    drawParameter.SetRenderSeaLand(renderSea);

    // To get accurate label drawing at tile borders, we take into account labels
    // of other than the current tile, too.
    if (zoom_level >= 14)
        // but not for high zoom levels, it is too expensive
        drawParameter.SetDropNotVisiblePointLabels(true);
    else
        drawParameter.SetDropNotVisiblePointLabels(false);

    // https://github.com/Framstag/libosmscout/blob/master/Documentation/RenderTuning.txt
    //searchParameter.SetBreaker(dataLoadingBreaker);
    if (magnification.GetLevel() >= 15)
        searchParameter.SetMaximumAreaLevel(6);
    else
        searchParameter.SetMaximumAreaLevel(4);

    // multithreading is realized by performing calls to
    // this function from multiple threads
    searchParameter.SetUseMultithreading(false);
    searchParameter.SetUseLowZoomOptimization(true);

    projection.Set(osmscout::GeoCoord(lat,lon),
                   0,
                   magnification,
                   dpi,
                   width,
                   height);

    std::cout << "rm: " << dpi << " " << " " << zoom_level << " " << width << "x" << height << " " << lat << " " << lon << "\n";

    projection.SetLinearInterpolationUsage(zoom_level >= 10);

    {
        QMutexLocker lk(&m_mutex);

        std::list<osmscout::TileRef> tiles;

        m_map_service->LookupTiles(projection,tiles);
        m_map_service->LoadMissingTileData(searchParameter,*m_style_config,tiles);
        m_map_service->ConvertTilesToMapData(tiles,data);

        if (drawParameter.GetRenderSeaLand())
            m_map_service->GetGroundTiles(projection, data.groundTiles);
    }

    //    QPixmap *pixmap=new QPixmap(width,height);
    //    if (pixmap == NULL)
    //        return false;
    QImage image(width,height,QImage::Format_RGB32);

    QPainter* painter=new QPainter(&image);
    if (painter == nullptr)
        return false;

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setRenderHint(QPainter::TextAntialiasing);
    painter->setRenderHint(QPainter::SmoothPixmapTransform);

    bool success = false;

    osmscout::MapPainterQt mapPainter(m_style_config);
    if (mapPainter.DrawMap(projection,
                           drawParameter,
                           data,
                           painter))
    {
        QBuffer buffer(&result);
        buffer.open(QIODevice::WriteOnly);
        image.save(&buffer, "PNG");
        success = true;
    }

    delete painter;

    return success;
}

#endif