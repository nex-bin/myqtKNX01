#include "mainwindow.h"

#include <QApplication>
#include "QBreakpadHandler.h"

int main(int argc, char *argv[])
{
    qDebug() << "main() 开始";

    QLockFile lockFile(QDir::temp().absoluteFilePath("myapp.lock"));

    if(lockFile.tryLock(100)){
        qDebug()<<"程序正常退出了";

    }
    else {
        // 锁被占用，但可能是上次崩溃留下的过期锁
        // QLockFile 会自动检测并清理过期锁，所以这里通常就是“另一个实例在运行”
        qWarning() << "程序已在运行";
        if(lockFile.error() == QLockFile::LockFailedError){
            qint64 pid;
            QString hostname;
            QString appname;

            qDebug()<<lockFile.getLockInfo(&pid,&hostname,&appname);
            if (true) {
                qDebug() << "程序已在运行！占用进程信息：";
                qDebug() << "  PID:" << pid;
                qDebug() << "  主机名:" << hostname;
                qDebug() << "  应用名:" << appname;
            }
        }

        // return 1;
    }

    QString dumpPath = QCoreApplication::applicationDirPath() + "/crash_dumps";
    QBreakpadInstance.setDumpPath(dumpPath);
    // QBreakpadInstance.setDumpPath("crash_dumps");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return QApplication::exec();

}
