// @Title  api表示层
// @Description  该文件主要用于向外提供api请求
package router

import (
	"github.com/labstack/echo/v4"
	"tcp/api/server"
)

// 智能家居相关的api
func SmartRouter(e *echo.Echo)  {
	const base = "api/v1/smart/"
	// 用户开锁操作
	e.GET(base+"open/door", server.SmartOpenDoor)
	// 用户权限录入
	e.POST(base+"add/user", server.SmartAddUser)
	// 判断用户是否认证
	e.GET(base+"auth/user", server.SmartAuthUser)
	// 微信小程序用户获取openid
	e.GET(base+"get/openid", server.SmartGetOpenId)
	// 微信小程序用户获取openid的 智联云家版本
	e.GET(base+"get/home/openid", server.SmartGetHomeOpenId)
	// 获取实验室当前的温度和湿度
	e.GET(base+"get/temp", server.SmartGetTemp)
	// 设置空调温度
	e.POST(base+"set/temp", server.SmartSetTemp)
	// 窗帘开关
	e.POST(base+"change/curtain", server.SmartChangeCurtain)
	// 饮水机开关
	e.POST(base+"change/drink", server.SmartChangeDrink)
	// 灯光开关
	e.POST(base+"change/light", server.SmartChangeLight)
}


