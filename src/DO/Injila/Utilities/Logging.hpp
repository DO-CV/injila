#pragma once

#include <QDebug>


#define INJILA_LOG() \
  qDebug() << "[INFO:  FILE:" << __FILE__ << ": LINE:" << __LINE__ << "]"

#define INJILA_ERR() \
  qDebug() << "[ERROR: FILE:" << __FILE__ << ": LINE:" << __LINE__ << "]"
