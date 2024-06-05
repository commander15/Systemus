namespace Systemus {

bool DatabaseDriver::setBackend(const QString &backend)
{
    if (this->backend == backend)
        return true;

    QSqlDriver *driver = loadDriver(backend);
    if (!driver)
        return false;
    
    if (this->driver)
        this->driver.reset(driver);
        
    this->backend = backend;
    return true;
}

QSqlDriver *DatabaseDriver::loadDriver(const QString &name)
{    
    QString pluginPath = "sqldrivers/qsql" + name.toLowerCase();
    QPluginLoader loader(pluginPath);
    QObject *plugin = loader.instance();
    if (!plugin) {
        qDebug() << "Failed to load plugin:" << loader.errorString();
        return nullptr;
    }

    QSqlDriver *driver = qobject_cast<QSqlDriver *>(plugin);
    if (!driver) {
        qDebug() << "Failed to cast plugin to QSqlDriver";
        return nullptr;
    }
    
    return driver;
}

bool DatabaseResult::exec()
{
    //...
    return result->exec();
}

}
