/*
 * GuLinux Planetary Imager - https://github.com/GuLinux/PlanetaryImager
 * Copyright (C) 2017  Marco Gulino <marco@gulinux.net>
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "scriptingengine.h"
#include <QtQml/QJSEngine>
#include <QDebug>
#include "protocol/scriptingprotocol.h"

using namespace std;

class ScriptingPlanetaryImager : public QObject {
  Q_OBJECT
public:
  ScriptingPlanetaryImager(const Configuration::ptr &configuration, const SaveImages::ptr &saveImages, QJSEngine &jsEngine);
  void setImager(Imager *imager);
  
public slots:
  void startRecording();
  void stopRecording();
signals:
  void message(const QString &text);
  
private:
  Configuration::ptr configuration;
  SaveImages::ptr saveImages;
  QJSEngine &jsEngine;
  Imager *imager= nullptr;
  QJSValue jsValue;
  QJSValue imagerJsValue;
};

class ScriptingConsole : public QObject {
  Q_OBJECT
public:
  ScriptingConsole(const NetworkDispatcher::ptr &dispatcher, QObject *parent = nullptr) : QObject{parent}, dispatcher{dispatcher} {}
public slots:
  void log(const QJSValue &v);
private:
  NetworkDispatcher::ptr dispatcher;
};

void ScriptingConsole::log(const QJSValue& v)
{
  dispatcher->send(ScriptingProtocol::packetScriptReply() << v.toString());
}


DPTR_IMPL(ScriptingEngine) {
  ScriptingEngine *q;
  unique_ptr<ScriptingPlanetaryImager> scriptedImager;
  QJSEngine engine;
};


ScriptingPlanetaryImager::ScriptingPlanetaryImager(const Configuration::ptr& configuration, const SaveImages::ptr& saveImages, QJSEngine &jsEngine)
  : configuration{configuration}, saveImages{saveImages}, jsEngine{jsEngine}
{
  jsValue = jsEngine.newQObject(this);
  jsEngine.globalObject().setProperty("pi", jsValue);
}

void ScriptingPlanetaryImager::setImager(Imager* imager)
{
  qDebug() << "Setting imager object" << imager;
  this->imager = imager;
  if(imager) {
    imagerJsValue = jsEngine.newQObject(imager);
    jsValue.setProperty("imager", imagerJsValue);
  } else {
    jsValue.deleteProperty("imager");
  }
}



ScriptingEngine::ScriptingEngine(const Configuration::ptr &configuration, const SaveImages::ptr &saveImages, const NetworkDispatcher::ptr &dispatcher, QObject *parent) 
: QObject(parent),  NetworkReceiver{dispatcher}, dptr(this)
{
  d->scriptedImager = make_unique<ScriptingPlanetaryImager>(configuration, saveImages, d->engine);
  connect(d->scriptedImager.get(), &ScriptingPlanetaryImager::message, this, [=](const QString &s) { emit reply(s + "\n"); });
  QJSValue console = d->engine.newQObject(new ScriptingConsole(dispatcher, this));
  d->engine.globalObject().setProperty("console", console);
  register_handler(ScriptingProtocol::Script, [this](const NetworkPacket::ptr &packet) {
    run(packet->payloadVariant().toString());
  });
  connect(this, &ScriptingEngine::reply, this, [dispatcher](const QString &message) {
    dispatcher->send(ScriptingProtocol::packetScriptReply() << message);
  });
}

ScriptingEngine::~ScriptingEngine()
{
}

void ScriptingEngine::run(const QString& script)
{
  qDebug() << "Running script: " << script;
  QJSValue v = d->engine.evaluate(script);
  if(v.isError())
    emit reply(v.toString());
}

void ScriptingEngine::setImager(Imager* imager)
{
  d->scriptedImager->setImager(imager);
}

void ScriptingPlanetaryImager::startRecording()
{
  if(! imager) {
    emit message("Error: you must select a camera first");
    return;
  }
  saveImages->startRecording(imager);
}

void ScriptingPlanetaryImager::stopRecording()
{
  saveImages->endRecording();
}

#include "scriptingengine.moc"