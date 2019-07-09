using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace DataParse
{
    public enum DataType
    {
        BIN,
        BCD,
    }
    public enum ShowType
    {
        DEC,        // 10进制显示
        HEX,        // 16进制显示
        DOUBLE,     // 需指定整数字节数，小数位数：2,5  /  4,5  /  2,3  /  4,3
        STR_ASCII,
        STR_HEX,
        STR_DATE,   // 需指定日期格式：yyyy-MM-dd  /  yy-MM-dd
        STR_TIME,   // 需指定时间格式：yyyy-MM-dd HH:mm:ss  / yy-MM-dd HH:mm:ss  /  HH:mm:ss  /  HH:mm
        STR_CASE,   // 需指定每个case含义：0-读取，1-设置，其他-未知 / 0xAA-成功，0xAB-失败，... ，其他-未知
        STR_BIT     // 需指定每个位含义：1bit-应答标识/应答/不应答， 1bit-加密标识/加密/不加密，1bit-帧类型/命令帧/数据帧, 5bit-保留
    }

    public class DataField
    {
        string _name;
        uint _size;
        DataType _dataType;
        ShowType _showTpye;

        
    }

    public class DataItem
    {
        int _id;            // 数据项ID
        string _name;       // 数据项名
        byte[] _data;       // 数据项原始数据

        List<DataField> _fields;    // 字段列表
    }
}
