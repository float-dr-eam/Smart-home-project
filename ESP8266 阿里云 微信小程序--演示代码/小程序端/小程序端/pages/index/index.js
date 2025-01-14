import mqtt from'../../utils/mqtt.js';
const aliyunOpt = require('../../utils/aliyun/aliyun_connect.js');

let that = null;
Page({
    data:{
      LightSwitch:1,
     // temperature:10,
    //  SmokeSensorValue:10,
   //   Humidity:11,
   //   basketball:12,
    //  pingpong : 13,
    //  Name:"19物联网二班gcy",

      client:null,//记录重连的次数
      reconnectCounts:0,//MQTT连接的配置
      options:{
        protocolVersion: 4, //MQTT连接协议版本
        clean: false,
        reconnectPeriod: 1000, //1000毫秒，两次重新连接之间的间隔
        connectTimeout: 30 * 1000, //1000毫秒，两次重新连接之间的间隔
        resubscribe: true, //如果连接断开并重新连接，则会再次自动订阅已订阅的主题（默认true）
        clientId: '',
        password: '',
        username: '',
        Things1CMD(a){
          var status=a.detail.value
          var that =this
          if(status ==ture )
          {
            this.onClickOpen()
          }
          else
          {
            this.onClickClock()
          }
        }
        // temp:0,
        // humi:0,
        // light:0
      },
      aliyunInfo: {
        productKey: 'k1st6SO97yW', //阿里云连接的三元组 ，请自己替代为自己的产品信息!!
        deviceName: 'WECHAT', //阿里云连接的三元组 ，请自己替代为自己的产品信息!!
        deviceSecret: '05b51b7a46cb6b6c2b29ece188638ae6', //阿里云连接的三元组 ，请自己替代为自己的产品信息!!
        regionId: 'cn-shanghai', //阿里云连接的三元组 ，请自己替代为自己的产品信息!!
        pubTopic: '/k1st6SO97yW/WECHAT/user/WECHAT', //发布消息的主题
        // /user/thing/service/property/set
        subTopic: ' /k1st6SO97yW/WECHAT/user/get', //订阅消息的主题
      },
    },
onLoad:function(){
  that = this;
      let clientOpt = aliyunOpt.getAliyunIotMqttClient({
        productKey: that.data.aliyunInfo.productKey,
        deviceName: that.data.aliyunInfo.deviceName,
        deviceSecret: that.data.aliyunInfo.deviceSecret,
        regionId: that.data.aliyunInfo.regionId,
        port: that.data.aliyunInfo.port,
      });
      console.log("get data:" + JSON.stringify(clientOpt));
      let host = 'wxs://' + clientOpt.host;
      console.log("get data:" + JSON.stringify(clientOpt));
      this.setData({
        'options.clientId': clientOpt.clientId,
        'options.password': clientOpt.password,
        'options.username': clientOpt.username,
        // 'Humidity': this.data.Humidity,
        // temperature:JSON.parse(payload).currentTemperature,
        // 'Humidity':JSON.parse(payload).read,
        //Humidity:JSON.parse(clientOpt).Humidity,
      })
      console.log("this.data.options host:" + host);
      console.log("this.data.options data:" + JSON.stringify(this.data.options));
// 访问服务器
      this.data.client = mqtt.connect(host, this.data.options);
      this.data.client.on('connect', function (connack) {
        wx.showToast({
          title: '连接成功'
        })
      })

      that.data.client.on(
        "message", function (topic, payload) {
        console.log(" 收到 topic:" + topic + " , payload :" + payload)
        let msg = message.toString()
        console.log('收到消息:'+msg)
        let temp = JSON.parse(msg)
        that.setData({
          // mqValue:temp,
          //转换成JSON格式的数据进行读取
           temperature:temp.temperature,
          // Humidity:JSON.parse(payload).read,
        })

        // wx.showModal({
        //   content: " 收到topic:[" + topic + "], payload :[" + payload + "]",
        //   showCancel: false,
        // });
      })
      //服务器连接异常的回调
      that.data.client.on("error", function (error) {
        console.log(" 服务器 error 的回调" + error)

      })
      //服务器重连连接异常的回调
      that.data.client.on("reconnect", function () {
        console.log(" 服务器 reconnect的回调")

      })
      //服务器连接异常的回调
      that.data.client.on("offline", function (errr) {
        console.log(" 服务器offline的回调")
      })
    },
    onClickOpen(){
      that.sendCommond(1);
    },
    onClickOff(){
      that.sendCommond(0);
    },
    sendCommond(data){
      let sendData={
        LightSwitch:data,
      };
      this.data.client.subscribe(this.data.aliyunInfo.subTopic,function(err){
        if(!err){
          console.log("订阅成功");
        };
        wx.showModal({
          content:"订阅成功",
          showCancel:false,
        })
      })
 // 发布消息
 if(this.data.client &&this.data.client.connected){
   this.data.client.publish.publish(this.data.aliyunInfo.pubTopic,JSON.stringify(sendData));
   console.log(this.data.aliyunInfo.pubTopic)
   console.log(Json.stringify(sendData))
 }else{
  wx.showToast({
    title: '请先连接服务器',
    icon:'none',
    duration:2000
  } ) 
 }
}
//   
// import MQTT from '../../utils/mqtt.js';

// Page({
//  data: {
//    temperature: '',
//    Humidity: '',
//  },
//  onLoad: function () {
//    // 初始化 MQTT 客户端
//    let client = MQTT.createMqttClient({
//      productKey: 'a12Fcmiorbf',
//      deviceName: 'device_2',
//      deviceSecret: '9557a9af4c2c4259b00a2a3cdf711098',
//      regionId: 'cn-shanghai',
//    });

//    // 连接到阿里云的 IoT 服务器
//    client.connect();

//    // 订阅温度和湿度的主题
//    client.subscribe({
//      topic: '/a12Fcmiorbf/device_2/user/thing/service/property/set',
//      qos: 1,
//      callback: function (err, granted) {
//        if (err) {
//          console.error('订阅失败:', err);
//          return;
//        }
//        console.log('订阅成功');
//      },
//    });

//    // 监听温度和湿度的数据
//    client.on('message', function (topic, message) {
//      let data = JSON.parse(message);
//      this.setData({
//        temperature: data.temperature,
//        Humidity: data.humidity,
//      });
//    });
//  },
 });