//app.js
// 引入工具文件夹
import tools from '/utils/tools.js';
App({
  // 设置全局变量
  globalData: {
    userInfo: null // 用户的个人信息
  },
  onLaunch: function () {
    // 我们需要手动赋值才可以发送请求
    this.globalData.SERVER="https://api.maker-iot.net"
    //this.globalData.SERVER="http://127.0.0.1:2334"
    // 设置自定义导航栏
    wx.getSystemInfo({
      success: e => {
        this.globalData.StatusBar = e.statusBarHeight;
        let custom = wx.getMenuButtonBoundingClientRect();
        this.globalData.Custom = custom;
        this.globalData.CustomBar = custom.bottom + custom.top - e.statusBarHeight;
      }
    })
    // 登录
    wx.login({
      success: res => {
        // 发送 res.code 到后台换取 openId, sessionKey, unionId
        tools.requests(this.globalData.SERVER + '/api/v1/smart/get/home/openid', { code: res.code }, 'get').then((res) => {
          if (res.code == 1) {
            const data=res.data
            wx.setStorageSync("openid", data.openid)
            wx.setStorageSync("session_key", data.session_key)
          }
        })
      }
    })
    // 获取用户信息
    wx.getSetting({
      success: res => {
        // 判断用户是否授权
        if (res.authSetting['scope.userInfo']) {
          // 已经授权，可以直接调用 getUserInfo 获取头像昵称，不会弹框
          wx.getUserInfo({
            success: res => {
              // 授权信息赋值给全局变量
              this.globalData.userInfo = res.userInfo
              console.log('个人信息',this.globalData.userInfo)
              // 由于 getUserInfo 是网络请求，可能会在 Page.onLoad 之后才返回
              // 所以此处加入 callback 以防止这种情况
              if (this.userInfoReadyCallback) {
                this.userInfoReadyCallback(res)
              }
            }
          })
        }
      }
    })
  },
  globalData: {
    userInfo: null
  }
})