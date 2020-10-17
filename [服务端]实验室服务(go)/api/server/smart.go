// @Title  api服务控制层
// @Description  这里主要负责处理数据，并操作数据库
package server

import (
	"encoding/json"
	"fmt"
	"github.com/labstack/echo/v4"
	"tcp/api/model"
	"tcp/api/tool"
	"tcp/common"
	"tcp/database"
	"tcp/tcp/hardware"
	"time"
)

// 开门函数
func SmartOpenDoor(c echo.Context) error {
	openid:=c.FormValue("openid")
	if openid==""{
		return model.DisplayError(c,"请先登录才能进行操作")
	}
	// 判断openid是否在数据库中
	if !tool.AuthUser(openid){
		return model.DisplayError(c,"你没有这个权限，请进行身份认证")
	} else {
		//select 的特点是只要其中一个 case 已经完成，程序就会继续往下执行，而不会考虑其他 case 的情况。
		// 向tcp服务器发送请求(door是门锁)
		select {
		case hardware.DoorChain <- "0103m": // 我们把这个数据放入管道 0103m是开锁命令
			// 这里发送数据成功，下面我们就从管道中接收数据，判断门锁是否开锁成功
			select {
			case data := <-hardware.DoorOpenChain:
				fmt.Println("get door",data)
				// 判断门锁返回的数据是否正确
				if data == "0110" {
					// 记录开锁时间
					_:database.InsertData(&database.UnlockRecord{Openid: openid,Lock: time.Now()})
					return model.DisplayOk(c, nil, "开锁成功")
				}
				return model.DisplayError(c, "开锁失败")
			case <-time.After(4 * time.Second): //我们这里设置4秒的超时时间，如果超时就说明门锁无反应
				return model.DisplayError(c, "门锁无反应")
			}
		case <-time.After(4 * time.Second): //我们这里设置4秒的超时时间，如果超时就说明门锁没有联网
			return model.DisplayError(c, "请检查门锁是否联网")
		}
	}
}

// 添加认证用户
func SmartAddUser(c echo.Context) error {
	// 获取必要数据
	openid:=c.FormValue("openid")
	nickname:=c.FormValue("nickname")
	avatar:=c.FormValue("avatar")
	name:=c.FormValue("name")
	group:=c.FormValue("group")
	token:=c.FormValue("token") // 授权码
	if openid == ""{
		return model.DisplayError(c,"请先登录才能进行操作")
	}
	// 判断授权码是否正确
	if token!="maker1024"{
		return model.DisplayError(c,"授权码错误")
	}
	// 初始化数据并插入
	user:=new(database.UserInfo)
	user.Openid = openid
	user.Nickname = nickname
	user.Avatar = avatar
	user.Name = name
	user.Group =group
	user.Authority = 2
	// 插入数据
	if database.InsertData(user) ==nil {
		return model.DisplayOk(c,nil,"认证成功")
	} else {
		return model.DisplayOk(c,nil,"认证失败")
	}
}

// 判断用户是否认证
func SmartAuthUser(c echo.Context) error {
	// 获取必要数据
	openid:=c.FormValue("openid")
	if openid == ""{
		return model.DisplayError(c,"请先登录才能进行操作")
	}
	// 判断用户是否热证
	if tool.AuthUser(openid){
		return model.DisplayOk(c,nil,"你已认证")
	} else {
		return model.DisplayError(c,"你还没有认证")
	}
}

// maker小程序获取openid
func SmartGetOpenId(c echo.Context) error {
	code := c.FormValue("code")
	if code == "" {
		return model.DisplayError(c,"参数错误")
	}
	//读取小程序配置
	//发送请求
	result := common.HttpGet("https://api.weixin.qq.com/sns/jscode2session?appid=wxaad49fb5b5ed1222&secret=3426f589a0bde161b399deb5e377b7f1&js_code=" + code + "&grant_type=authorization_code")
	//解析获取到的json数据
	var v interface{}
	err := json.Unmarshal([]byte(result), &v)
	if err != nil {
		return model.DisplayError(c,"获取用户信息失败")
	}
	return model.DisplayOk(c,v,"获取用户数成功")
}

// 智联云家获取appid
func SmartGetHomeOpenId(c echo.Context) error {
	code := c.FormValue("code")
	if code == "" {
		return model.DisplayError(c,"参数错误")
	}
	//读取小程序配置
	//发送请求 e2fc066cfee969dc61200c0209bbc03f
	result := common.HttpGet("https://api.weixin.qq.com/sns/jscode2session?appid=wx45d4e3763235a687&secret=e2fc066cfee969dc61200c0209bbc03f&js_code=" + code + "&grant_type=authorization_code")
	//解析获取到的json数据
	var v interface{}
	err := json.Unmarshal([]byte(result), &v)
	if err != nil {
		return model.DisplayError(c,"获取用户信息失败")
	}
	return model.DisplayOk(c,v,"获取用户数成功")
}

// 获取当前的温度信息（温湿度，空调，窗帘，饮水机）
func SmartGetTemp(c echo.Context) error {
	temp:=model.SmartTemp{
		Temperature: hardware.Temperature,
		Humidity: hardware.Humidity,
		Air: hardware.Air,
		Curtain: hardware.Curtain,
		Drink: hardware.Drink,
		Light: hardware.Light,
	}
	return model.DisplayOk(c,temp,"获取温度数据成功")
}

// 获取当前空调的温度
func SmartSetTemp(c echo.Context) error {
	if !tool.AuthReturn(c){
		return model.DisplayError(c,"你没有这个权限")
	}
	// 获取设置的温度的值
	temp:=c.FormValue("temp")
	if temp==""{
		return model.DisplayError(c,"参数错误")
	}
	// 向管道中添加数据(直接把温度数据发送过去)
	hardware.TempChain <- temp + "m"
	return model.DisplayOk(c,nil,"设置温度数据成功，请等待后台响应")
}

// 修改窗帘状态
func SmartChangeCurtain(c echo.Context) error {
	if !tool.AuthReturn(c){
		return model.DisplayError(c,"你没有这个权限")
	}
	// 判断是否要开窗帘 1 是开 0 是关
	open:=c.FormValue("open")
	if open==""{
		return model.DisplayError(c,"参数错误")
	}
	var ok bool
	if open == "1"{
		fmt.Println("开窗帘")
		ok = hardware.SendData("C1m")
		//hardware.Curtain = true
	} else {
		ok = hardware.SendData("C0m")
	}
	if ok{
		return model.DisplayOk(c,nil,"操作成功")
	} else {
		return model.DisplayError(c,"检查设备是否联网")
	}
}

// 修改饮水机状态
func SmartChangeDrink(c echo.Context) error {
	if !tool.AuthReturn(c) {
		return model.DisplayError(c, "你没有这个权限")
	}
	//return model.DisplayError(c,"饮水机还没上线")
	// 判断是否要开窗帘 1 是开 0 是关
	open := c.FormValue("open")
	if open == "" {
		return model.DisplayError(c, "参数错误")
	}
	var ok bool
	if open == "1"{
		fmt.Println("开窗帘")
		ok = hardware.SendData("D1m")
		//hardware.Curtain = true
	} else {
		ok = hardware.SendData("D0m")
	}
	if ok{
		return model.DisplayOk(c,nil,"操作成功")
	} else {
		return model.DisplayError(c,"检查设备是否联网")
	}
}

// 修改灯光状态
func SmartChangeLight(c echo.Context) error {
	if !tool.AuthReturn(c){
		return model.DisplayError(c,"你没有这个权限")
	}
	//return model.DisplayError(c,"饮水机还没上线")
	// 判断是否要开窗帘 1 是开 0 是关
	open:=c.FormValue("open")
	if open==""{
		return model.DisplayError(c,"参数错误")
	}
	var ok bool
	if open == "1"{
		fmt.Println("开灯")
		ok = hardware.SendData("L1m")
		//hardware.Curtain = true
	} else {
		ok = hardware.SendData("L0m")
	}
	if ok{
		return model.DisplayOk(c,nil,"操作成功")
	} else {
		return model.DisplayError(c,"检查设备是否联网")
	}
}