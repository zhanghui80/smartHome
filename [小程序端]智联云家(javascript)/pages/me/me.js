// pages/me/me.js
const app = getApp()
import tools from '../../utils/tools.js';
import Toast from '../../miniprogram_npm/@vant/weapp/toast/toast';
import Dialog from '../../miniprogram_npm/@vant/weapp/dialog/dialog';
Page({

  /**
   * 页面的初始数据
   */
  data: {
    userInfo: {},
    hasUserInfo: false,
    canIUse: wx.canIUse('button.open-type.getUserInfo')
  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function (options) {
    // 主动获取用户信息
    if (app.globalData.userInfo) {
      this.setData({
        userInfo: app.globalData.userInfo,
        hasUserInfo: true
      })
    } else if (this.data.canIUse){
      // 由于 getUserInfo 是网络请求，可能会在 Page.onLoad 之后才返回
      // 所以此处加入 callback 以防止这种情况
      app.userInfoReadyCallback = res => {
        this.setData({
          userInfo: res.userInfo,
          hasUserInfo: true
        })
      }
    } else {
      // 在没有 open-type=getUserInfo 版本的兼容处理
      wx.getUserInfo({
        success: res => {
          app.globalData.userInfo = res.userInfo
          this.setData({
            userInfo: res.userInfo,
            hasUserInfo: true
          })
        }
      })
    }
  },
  getUserInfo: function(e) {
    console.log(e)
    app.globalData.userInfo = e.detail.userInfo
    // 判断是否获取到了个人信息
    if (e.detail.errMsg === 'getUserInfo:ok'){
      this.setData({
        userInfo: e.detail.userInfo,
        hasUserInfo: true
      })
    }
  },
  showProcess: function(e) {
    Toast('开发中~');
  },
  authUser() {
    // 先判断用户是否为授权用户
    let data = {}
    try {
      data = {
        openid: wx.getStorageSync('openid'),
      }
    } catch (e) {
      wx.showToast({title: '出现意外请重试', icon: 'none'})
    }
    // 发送请求进行身份认证
    tools.requests(app.globalData.SERVER + '/api/v1/smart/auth/user', data, 'get').then((res) => {
      if(res.code == 1){
        wx.showToast({title: '你已认证'})
      } else {
        // 先判断openid是否为空
        if (!data.openid){
          wx.showToast({title: res.msg, icon: 'none'})
        } else {
          // 跳转到权限认证页面
          wx.navigateTo({url: '/pages/me/auth/auth'})
        }
      }
    })
  },
  cocpyGithub(){ // 复制github组织链接
    wx.setClipboardData({
      data: 'https://github.com/Maker-IoT-hnust',
      success (res) {
        wx.getClipboardData({
          success (res) {
            console.log(res.data) // data
          }
        })
      }
    })
  },
  priceAuthor(){ // 赞赏支持作者
    wx.previewImage({
      current: 'https://img.xiaoyou66.com/images/2020/09/29/6GQH.png', // 当前显示图片的http链接
      urls: ['https://img.xiaoyou66.com/images/2020/09/29/6GQH.png'] // 需要预览的图片http链接列表
    })
  },
  aboutAuthor () { // 关于作者
    Dialog.confirm({
      title: '关于作者',
      message: '作者:小游\n作者博客:xiaoyou66.com\n作者B站uid:343147393',
      showConfirmButton: false,
      showCancelButton:false,
      closeOnClickOverlay:true
    }).then(() => {
    }).catch(()=>{
    })
  }
})