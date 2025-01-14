// index.js
const defaultAvatarUrl = 'https://mmbiz.qpic.cn/mmbiz/icTdbqWNOwNRna42FI242Lcia07jQodd2FJGIYQfG0LAJGFxM4FbnQP6yfMxBgJ0F3YRqJCJ1aPAK2dQagdusBZg/0'
const KEY = "c69e0046732ffd0e29105049995043d0"
var mqtt = require('../../utils/mqtt.min.js')
const crypto = require('../../utils/crypto');


Page({
  //顶部标题栏
  data: {
    motto: 'Hello World',
    title:'Intellectual HouseSystem',
    //***温度传感器***//
    top_tem: 50,
    bottem_tem: 0,
    currenttemperature:30,
    //***湿度传感器***//
    top_humidity: 100,
    bottem_humidity: 40,
    currenthumidity:60,
    //******水位******/
    currentwaterlevel: 50, // 1 表示正常，2 表示警告
    waterflag: 1,
    waterFlagColors: {
      1: 'green',   // 正常时显示绿色
      2: 'orange',
      3: 'red' ,     // 危险时显示红色
    },
   //*******开关初始化******// 
   isLedOn: false, // LED 初始状态为关闭
   isjiashiOn: false, // 加湿器初始状态为关闭
   isalertOn: 0,
    mqttAccountInfo: {
      /*productKey: "k23u6jUzTHa",
      deviceName: "appdevice",
      deviceSecret: "798361fe68d29ec57f4c65e1d2f2e7a5",/*/
      productKey: 'k1st6SO97yW', //阿里云连接的三元组 ，请自己替代为自己的产品信息!!
      deviceName: 'WECHAT', //阿里云连接的三元组 ，请自己替代为自己的产品信息!!
      deviceSecret: '05b51b7a46cb6b6c2b29ece188638ae6', //阿里云连接的三元组 ，请自己替代为自己的产品信息!!
      regionId: 'cn-shanghai', //阿里云连接的三元组 ，请自己替代为自己的产品信息!!
      pubTopic: '/k1st6SO97yW/WECHAT/user/WECHAT', //发布消息的主题
      subTopic: '/k1st6SO97yW/WECHAT/user/get', //订阅消息的主题
    },                                                 
    sendData: {}  // 发送的数据对象
  },

  updateWaterFlag: function() {
    let currentwaterlevel = this.data.currentwaterlevel;
    let waterflag;

    if (currentwaterlevel < 50) {
      waterflag = 3; // 低水位
    } else if (currentwaterlevel >= 50 && currentwaterlevel <= 80) {
      waterflag = 2; // 中等水位
    } else {
      waterflag = 1; // 高水位
    }
    // 更新 waterflag
    this.setData({
      waterflag: waterflag
    });
  },

  //获取位置信息
  onLoad() {
    this.getLocation()
    this.doConnect()
    this.updateWaterFlag()
  },

  getLocation() {
    let that = this
    wx.getLocation({
      type: 'wgs84',
      success(res) {
        console.log(res)
        const latitude = res.latitude
        const longitude = res.longitude
        const speed = res.speed
        const accuracy = res.accuracy

      wx.request({
        url: 'https://restapi.amap.com/v3/geocode/regeo?parameters',
        data:{
          key:KEY,
          location: longitude + "," + latitude
        },
        success(res){
          console.log(res)
          let cityCode = res.data.regeocode.addressComponent.adcode
          wx.request({
            url: 'https://restapi.amap.com/v3/weather/weatherInfo?parameters',
            data:{
              key:KEY,
              city:cityCode
            },
            success(res){
            console.log(res)
            let weather_info = res.data.lives[0]
            weather_info.imageUrl = "/images/weather/" + that.getIconKey(weather_info.weather) + ".png"
            console.log(weather_info)
            that.setData({
              weatherInfo: weather_info
            })
            }
          })
        }
      })
      }
    })
  },

  getIconKey(weather) {
    const iconWeatherMap = {
      '风': ['有风', '平静', '微风', '和风', '清风', '强风/劲风', '疾风', '大风', '烈风', '风暴', '狂爆风', '飓风', '热带风暴', '龙卷风'],
      '多云': ['少云', '晴间多云', '多云'],
      '雪': ['雪', '阵雪', '小雪', '中雪', '大雪', '暴雪', '小雪-中雪', '中雪-大雪', '大雪-暴雪', '冷'],
      '雾': ['浮尘', '扬沙', '沙尘暴', '强沙尘暴', '雾', '浓雾', '强浓雾', '轻雾', '大雾', '特强浓雾'],
      '晴': ['晴', '热'],
      '雨夹雪': ['雨雪天气', '雨夹雪', '阵雨夹雪'],
      '雨': ['阵雨', '雷阵雨', '雷阵雨并伴有冰雹', '小雨', '中雨', '大雨', '暴雨', '大暴雨', '特大暴雨', '强阵雨', '强雷阵雨', '极端降雨', '毛毛雨/细雨', '雨', '小雨-中雨', '中雨-大雨', '大雨-暴雨', '暴雨-大暴雨', '大暴雨-特大暴雨', '冻雨'],
      '阴': ['阴', '霾', '中度霾', '重度霾', '严重霾', '未知']
    }
    // 这个是默认值
    let iconKey = '阴'
    for (const weatherKey in iconWeatherMap) {
      if (Object.hasOwnProperty.call(iconWeatherMap, weatherKey)) {
        const weatherNames = iconWeatherMap[weatherKey]
        const findWeatherItem = weatherNames.find(name => weather === name)
        if (findWeatherItem) {
          iconKey = weatherKey
          break
        }
      }
    }
    return iconKey
  },

  doConnect() {
    let that = this;
    const options = this.initMqttOptions();
    this.data.client = mqtt.connect('wxs://' + this.data.mqttAccountInfo.productKey + '.iot-as-mqtt.cn-shanghai.aliyuncs.com', options);
  
    this.data.client.on('connect', function () {
      console.log('连接服务器成功');
      that.data.client.subscribe('/' + that.data.mqttAccountInfo.productKey + '/' + that.data.mqttAccountInfo.deviceName + '/user/get', function (err) {
        if (!err) {
          console.log('订阅成功！');
        }
      });
    });
  
    // 接收消息监听
    this.data.client.on('message', function (topic, message) {
      let msg = message.toString();
      console.log('收到消息：' + msg);
      try {
        // 尝试解析 JSON 消息
        const parsedMessage = JSON.parse(msg);
        // 更新当前温度
        if (parsedMessage.currentTemperature !== undefined) {
          that.setData({
            currenttemperature: parseFloat(parsedMessage.currentTemperature)
          });
        }

        if (parsedMessage.top_tem !== undefined) {
          that.setData({
            top_tem: parseFloat(parsedMessage.top_tem)
          });
        }

        if (parsedMessage.bottem_tem !== undefined) {
          that.setData({
            bottem_tem: parseFloat(parsedMessage.bottem_tem)
          });
        }
  
        // 更新当前湿度
        if (parsedMessage.currentHumidity !== undefined) {
          that.setData({
            currenthumidity: parseFloat(parsedMessage.currentHumidity)
          });
        }
        
        if (parsedMessage.bottem_humidity !== undefined) {
          that.setData({
            bottem_humidity: parseFloat(parsedMessage.bottem_humidity)
          });
        }
        // 更新当前水位
        if (parsedMessage.currentwaterlevel !== undefined) {
          that.setData({
            currentwaterlevel: parseInt(parsedMessage.currentwaterlevel)
          });
        }

        if (parsedMessage.alertflag !== undefined) {
          that.setData({
            isalertOn: parseInt(parsedMessage.alertflag)
          });  
        }
        
        //更新当前开关状态
        if (parsedMessage.ledflag !== undefined) {
          that.setData({
            isLedOn: parseInt(parsedMessage.ledflag)
          });
        }
        if (parsedMessage.jiashiflag !== undefined) {
          that.setData({
            isjiashiOn: parseInt(parsedMessage.jiashiflag)
          });
        }
        // 其他传感器数据可以根据需要继续添加
        // that.setData({
        //   sensorList: parsedMessage
        // });
      } catch (error) {
        console.error('消息解析失败:', error);
      }
    });
  },
unconnect() {
  this.data.client.end();
  this.data.client = null;
  console.log("断开连接")
},

// 处理开关状态变化
handleSwitchChange: function (e) {
  const { id } = e.currentTarget.dataset; // 获取开关的标识符
  const value = e.detail.value; // 获取开关的当前状态

  // 根据开关的标识符更新相应的状态
  if (id === 'led') {
    this.setData({ isLedOn: value });
  } else if (id === 'jiashi') {
    this.setData({ isjiashiOn: value });
  }
  const sendData = {
      ledflag: this.data.isLedOn ? 1 : 0, // LED 状态
      jiashiflag: this.data.isjiashiOn ? 1 : 0, // 加湿器状态
  };
  this.publish(sendData);
},

publish: function (data) {
  let sendData = {
    //cmd: cmd,
    data: data,
  };
  // 判断是否已成功连接
  if (!this.data.client || !this.data.client.connected) {
    console.log('发布失败：未连接到服务器');
    return;
  }
  else if (this.data.client && this.data.client.connected) {
    this.data.client.publish(this.data.mqttAccountInfo.pubTopic, JSON.stringify(sendData));
    console.log(this.data.mqttAccountInfo.pubTopic)
    console.log(JSON.stringify(sendData))
  }
},

//IoT平台mqtt连接参数初始化
initMqttOptions() {
  const params = {
    productKey: this.data.mqttAccountInfo.productKey,
    deviceName: this.data.mqttAccountInfo.deviceName,
    deviceSecret: this.data.mqttAccountInfo.deviceSecret,
    timestamp: Date.now(),
  }
  //生成clientId，username，password
  const contentStr = "clientId" + params.clientId + "deviceName" + params.deviceName + "productKey" + params.productKey + "timestamp" + params.timestamp;
  const clientId = `${params.clientId}|securemode=2,signmethod=hmacsha1,timestamp=${params.timestamp}|`;
  const username = `${params.deviceName}&${params.productKey}`;
  const password = crypto.HmacSHA1(contentStr, params.deviceSecret).toString();
  const options = {
    wsOptions: {},
    protocolVersion: 4, //MQTT连接协议版本
    clientId: clientId,
    keepalive: 60,
    clean: false,
    username: username,
    password: password,
    reconnectPeriod: 1000, //1000毫秒，两次重新连接之间的间隔
    connectTimeout: 30 * 1000, //1000毫秒，两次重新连接之间的间隔
    resubscribe: true //如果连接断开并重新连接，则会再次自动订阅已订阅的主题（默认true）
  }
  return options;
},

})