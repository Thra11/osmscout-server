/*
 * Copyright (C) 2016-2018 Rinigus https://github.com/rinigus
 * 
 * This file is part of OSM Scout Server.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef MAPMANAGER_DELETERTHREAD_H
#define MAPMANAGER_DELETERTHREAD_H

#include <QThread>
#include <QStringList>
#include <QDir>

namespace MapManager {

  class DeleterThread : public QThread
  {
    Q_OBJECT
  public:
    DeleterThread(QObject *parent,
                  QDir &root_dir,
                  const QStringList &todelete):
      QThread(parent),
      m_root_dir(root_dir),
      m_todelete(todelete)
    {}

  protected:
    virtual void run() override;

  protected:
    QDir m_root_dir;
    QStringList m_todelete;
  };

}
#endif // DELETERTHREAD_H
