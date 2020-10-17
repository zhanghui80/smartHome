// @Title  数据库模型层
// @Description  这里主要放数据的持久层结构
package database

import "time"

// 存储个人信息
type UserInfo struct {
	ID int `gorm:"primaryKey"` // 用户专属id
	Openid string  // 用户的openid
	Nickname string // 用户微信昵称
	Avatar string // 用户个人头像
	Name string  // 用户的名字
	Group string // 用户所属的组
	Authority int // 用户的权限 1 是管理员 2是普通成员
}

// 开锁记录
type UnlockRecord struct {
	ID int `gorm:"primaryKey"` // 用户专属id
	Openid string // 用户开锁的openid
	Lock time.Time // 开锁时间
}