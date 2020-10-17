// @Title  请填写文件名称
// @Description  请填写文件描述
package main

import (
	"tcp/api"
	"tcp/database"
	"tcp/tcp"
	"tcp/tcp/hardware"
)

// 入口函数
func main()  {
	// 管道初始化
	hardware.DoorChain =make(chan string)
	hardware.DoorOpenChain =make(chan string)
	hardware.TempChain = make(chan string)
	hardware.CurtainChain = make(chan string)
	// 数据库初始化
	database.DbInit()
	// 创建各种表
	_ = database.CreateTable(database.UserInfo{})
	_ = database.CreateTable(database.UnlockRecord{})
	// 开启一个线程启动tcp服务
	go tcp.ServerStart()
	// 初始化api服务器
	api.WebInit()
}