// pages/me/auth/auth.js
import tools from '../../../utils/tools.js';
const app = getApp()
Page({

  /**
   * 页面的初始数据
   */
  data: {
    index: 4,
    name:'',
    token: '',
    picker: ['服务器组', '硬件组', '客户端组','策宣组','外来人员'],
  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function (options) {

  },
  PickerChange(e) {  // 单选选择事件
    this.setData({
      index: e.detail.value
    })
  },
  nameInput(e){ // 姓名输入框事件
    this.setData({
      name: e.detail.value
    })
  },
  tokenInput(e){ // 授权码事件
    this.setData({
      token: e.detail.value
    })
  },
  startAuth(e){ // 开始进行权限认证
    // 获取一些必要信息
    let data = {}
    try {
      data = {
        openid: wx.getStorageSync('openid'),
        nickname: app.globalData.userInfo.nickName,
        avatar: app.globalData.userInfo.avatarUrl,
        name: this.data.name,
        group: this.data.picker[this.data.index],
        token: this.data.token
      }
    } catch (e) {
      wx.showToast({title: '出现意外请重试', icon: 'none'})
    }
    tools.requests(app.globalData.SERVER + '/api/v1/smart/add/user', data, 'post').then((res) => {
      if (res.code === 1){
        wx.showToast({title: '认证成功'})
        setTimeout(()=>{
          // 跳转到个人中心页面
          wx.switchTab({url: '/pages/me/me'})
        },1000)
      } else {
        wx.showToast({title: res.msg, icon: 'none'})
      }
    })
  }
})