// @Title  窗帘控制
// @Description  主要是和窗帘有关的控制和处理函数
package hardware

import (
	"fmt"
	"net"
	"strings"
	"time"
)

var Curtain bool // 窗帘
var Drink bool // 饮水机
var Light bool // 灯光控制

var CurtainChain chan string

// 窗帘发送的处理函数
func CurtainSendProcess(conn net.Conn) bool  {
	// 我们这里设置超时机制
	select {
	case temp := <-CurtainChain:
		fmt.Println("发送数据",temp)
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


// 窗帘
func CurtainReceiveProcess(data string)  {
	// 说明是门锁发来的消息(我们直接返回门锁的数据)
	if strings.Split(data,",")[1] == "1"{
		Curtain = true
	} else {
		Curtain = false
	}
	fmt.Println("窗帘",data)
}
// 饮水机
func DrinkReceiveProcess(data string)  {
	// 说明是门锁发来的消息(我们直接返回门锁的数据)
	if strings.Split(data,",")[1] == "1"{
		Drink = true
	} else {
		Drink = false
	}
	fmt.Println("饮水机",data)
}
// 灯光
func LightReceiveProcess(data string)  {
	// 说明是门锁发来的消息(我们直接返回门锁的数据)
	if strings.Split(data,",")[1] == "1"{
		Light = true
	} else {
		Light = false
	}
	fmt.Println("灯",data)
}
// 管道数据传输
func SendData(data string) bool {
	select {
	case CurtainChain <- data: // 我们把这个数据放入传送的数据的管道
		return true
	case <-time.After(4 * time.Second): //我们这里设置4秒的超时时间，如果超时就说明设备没有联网
		return false
	}
}