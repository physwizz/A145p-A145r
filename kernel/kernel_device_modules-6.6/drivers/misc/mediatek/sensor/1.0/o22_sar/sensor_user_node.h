#ifndef _SENSOR_USER_NODE_H_
#define _SENSOR_USER_NODE_H_

#define NODE_NAME  "sensor_node"
/*A14_V code for SR-AL6528V-01-196 by hefan at 2023/09/23 start*/
struct sar_driver_func
{
    ssize_t (*set_onoff)(const char *buf, size_t count);
    ssize_t (*get_onoff)(char *buf);
    ssize_t (*set_dumpinfo)(const char *buf, size_t count);
    ssize_t (*get_dumpinfo)(char *buf);
};
/*A14_V code for SR-AL6528V-01-196 by hefan at 2023/09/23 end*/

extern struct sar_driver_func sar_func;

#endif