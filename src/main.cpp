#include <QApplication>
#include <QtGlobal>
#include <QTime>
#include <QSettings>

#include "mainwindow.h"

#ifdef QT_NO_DEBUG

#include <QFile>
#include <QString>
#include <QTextStream>

QString logFileName = QString("logfile.txt");

#endif

void debugMessageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
  QString message;
  switch (type) {
#ifdef QT_DEBUG
    case QtDebugMsg:
      message = QString("[D][%1]: %2 (%3 %4:%5)\n").arg(QTime::currentTime().toString()).arg(msg).arg(context.file).arg(context.function).arg(context.line);
      break;
#endif
    case QtInfoMsg:
      message = QString("[I][%1]: %2 (%3 %4:%5)\n").arg(QTime::currentTime().toString()).arg(msg).arg(context.file).arg(context.function).arg(context.line);
      break;
    case QtWarningMsg:
      message = QString("[W][%1]: %2 (%3 %4:%5)\n").arg(QTime::currentTime().toString()).arg(msg).arg(context.file).arg(context.function).arg(context.line);
      break;
    case QtCriticalMsg:
      message = QString("[C][%1]: %2 (%3 %4:%5)\n").arg(QTime::currentTime().toString()).arg(msg).arg(context.file).arg(context.function).arg(context.line);
      break;
    case QtFatalMsg:
      message = QString("[F][%1]: %2 (%3 %4:%5)\n").arg(QTime::currentTime().toString()).arg(msg).arg(context.file).arg(context.function).arg(context.line);
      break;
    default:
      return;
  }

#ifdef QT_DEBUG
  fprintf(stderr, message.toStdString().data());

  fflush(stderr);
  fflush(stdout);
#else
  QFile logFile(logFileName);
  logFile.open(QIODevice::Text | QIODevice::Append);
  QTextStream logStream(&logFile);
  logStream << message;
  logFile.close();
#endif

  if (type == QtFatalMsg)
    abort();
}

int main(int argc, char* argv[])
{
  qInstallMessageHandler(debugMessageOutput);
  QApplication a(argc, argv);

  QCoreApplication::setApplicationName("EVEBlueprints");
  QCoreApplication::setOrganizationName("Ierihon Inc.");

#ifdef QT_NO_DEBUG
  QFile logFile(logFileName);
  logFile.open(QIODevice::Text | QIODevice::WriteOnly);
  QTextStream logStream(&logFile);
  logStream << "--- LOG FILE --- \n\n";
#endif

  MainWindow w;
  w.show();

  return a.exec();
}
