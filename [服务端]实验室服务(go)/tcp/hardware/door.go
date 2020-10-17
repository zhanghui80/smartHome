// @Title  门锁硬件处理
// @Description  请填写文件描述
package hardware

import (
	"fmt"
	"net"
	"strings"
	"time"
)

// 门锁专属管道
var DoorChain chan string
// 开门通知管道（这个管道用于上报开门数据）
var DoorOpenChain chan string


// 发送数据处理函数
func DoorSendProcess(conn net.Conn) bool  {
	// 我们这里设置超时机制
	select {
	case answer := <-DoorChain:
		fmt.Println(time.Now(),"发送数据",answer)
		// 把获取到的数据发送过去
		if _,err:=conn.Write([]byte(answer));err!=nil{
			fmt.Println("发送数据失败",answer)
			// 如果发生数据失败，我们直接退出函数
			conn.Close()
			return false
		}
	case <-time.After(2*time.Second): //管道超时为2s
		// 这里发送一条心跳命令，用于监测当前客户端是否在线
		if _,err:=conn.Write([]byte("002m"));err!=nil{
			// 如果发生数据失败，我们直接退出函数
			conn.Close()
			return false
		}
	}
	return true
}

// 门锁接收数据处理函数
func DoorReceiveProcess(data string)  {
	// 获取数据，如果数据为0000，那么我们就不进行处理
	data=strings.Split(data,",")[1]
	if data == "0000"{
		return
	}
	fmt.Println(time.Now(),"门锁数据上报",data)
	// 否则我们直接返回门锁的数据
	DoorOpenChain <- data
}
