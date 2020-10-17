// @Title  TCP 处理函数
// @Description  这里负责处理TCP请求
package tcp

import (
	"fmt"
	"log"
	"net"
	"strings"
	"tcp/tcp/hardware"
	"time"
)

// 检查err是否错误，如果错误打印错误日志
func checkError(err error) {
	if err != nil {
		log.Fatal(err.Error())
	}
}

// 客户端请求处理函数
func clientHandle(conn net.Conn) {
	// 用于判断当前的设置类型（默认什么也不是）
	var clientType = 0
	// 打印远程地址
	fmt.Println(time.Now(),"new_conn:", conn.RemoteAddr().String())
	// 设置tcp超时时间为3分钟
	_ = conn.SetReadDeadline(time.Now().Add(time.Minute * 3))
	// 进程结束时自动关闭连接
	defer conn.Close()
	// 创建一个线程用来发送数据
	// 我们利用go的匿名函数来处理
	// 我们可以这样理解匿名函数末尾的括号，这里就相当于直接调用函数
	//var b = func a() { ............ }
	//b() //调用该函数
	go func(conn net.Conn) {
		for {
			// 判断链接是否断开(我们这里通过一个ok来进行数据判断)
			var ok bool
			// 判断当前设备的类型,执行不同的操作
			switch clientType {
			case 1:
				ok = hardware.DoorSendProcess(conn)
			case 2:
				ok = hardware.TempSendProcess(conn)
			case 3:
				// 注意go的switch默认带有break，所以我们需要使用fallthrough向下传递
				fallthrough
			case 4:
				fallthrough
			case 5:
				ok = hardware.CurtainSendProcess(conn)
			default:
				// 默认情况下我们发送一条心跳包
				if _,err:=conn.Write([]byte("000m"));err!=nil{
					// 如果发生数据失败，我们直接退出函数
					conn.Close()
					return
				}
				// 避免默认情况下自动退出
				ok = true
				// 延时1秒，发送默认心跳包
				time.Sleep(time.Second)
			}
			// 没有接收到数据，断开连接
			if !ok{
				return
			}
		}
	}(conn)
	// 同样使用for循环来处理客户发送的数据
	for {
		// 初始化一个字节数组，然后读取
		dataB := make([]byte, 1024)
		n, err := conn.Read(dataB)
		if n == 0 || err != nil {
			break
		}
		// 判断数据上报的类型，用来处理上报数据
		data:=string(dataB)
		if strings.Index(data,"$door,") != -1{
			clientType = 1
			hardware.DoorReceiveProcess(data)
		} else if strings.Index(data,"$temp,") != -1{
			clientType = 2
			hardware.TempReceiveProcess(data)
		} else if strings.Index(data,"$Curtain,") != -1{
			clientType = 3
			hardware.CurtainReceiveProcess(data)
		} else if strings.Index(data,"$Drinking,") != -1{
			clientType = 4
			hardware.DrinkReceiveProcess(data)
		} else if strings.Index(data,"$Light,") != -1{
			clientType = 5
			hardware.LightReceiveProcess(data)
		}
	}
}


//================================
// 启动tcp服务
func ServerStart() {
	// 创建一个tcp服务器,监听8084号端口
	tcpAddr, err := net.ResolveTCPAddr("tcp4", "0.0.0.0:8084")
	checkError(err)
	// 启动tcp服务器并监听端口
	listen, err := net.ListenTCP("tcp", tcpAddr)
	checkError(err)
	// 死循环处理客户端请求
	for {
		// 获取tcp请求
		conn, err := listen.Accept()
		// 有错误直接跳过不处理
		if err != nil {
			continue
		}
		// 启动线程来处理请求
		go clientHandle(conn)
	}
}