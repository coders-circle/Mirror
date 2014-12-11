
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/array.hpp>

#include <gtk/gtk.h>
#include <opencv2/opencv.hpp>


#include "Exception.h"

using boost::asio::ip::udp;
using boost::asio::ip::tcp;