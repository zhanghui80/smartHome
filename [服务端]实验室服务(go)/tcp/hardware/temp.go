// @Title  温湿度传感器硬件处理
// @Description  请填写文件描述
package hardware

import (
	"net"
	"strconv"
	"strings"
	"tcp/common"
	"time"
)

var Temperature float64 // 温度
var Humidity int // 湿度
var Air int // 空调当前的温度

// 设置空调温度的管道
var TempChain chan string

// 发送数据处理函数
func TempSendProcess(conn net.Conn) bool {
	// 我们这里设置超时机制
	select {
	case temp := <-TempChain:
		// 把温度数据转换为二进制数据
		// 把获取到的数据发送过去
		if _,err:=conn.Write([]byte(temp));err!=nil{
			// 如果发生数据失败，我们直接退出函数
			conn.Close()
			return false
		}
	case <-time.After(2*time.Second): //管道超时为2s
		// 这里发送一条心跳命令，用于监测当前客户端是否在线
		if _,err:=conn.Write([]byte("000m"));err!=nil{
			// 如果发生数据失败，我们直接退出函数
			conn.Close()
			return false
		}
	}
	return true
}

// 接收数据处理函数(我们这里直接把温度数据赋值给全局变量即可)
func TempReceiveProcess(data string)  {
	tmps := strings.Split(data,",")
	Temperature,_ = strconv.ParseFloat(tmps[1],64)
	Humidity = common.String2Int(tmps[2])
	Air = common.String2Int(tmps[3])
}