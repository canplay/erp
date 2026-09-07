export default {
  onShareAppMessage() {
    return {
      title: '七都商合',
      path: 'pages/index/index',
      imageUrl: '/static/logo.png',
      desc: '七都商合 | 示例城市A生活商业服务一站式平台',
    };
  },
  onShareTimeline() {
    return {
      title: '七都商合',
      query: 'pages/index/index',
      imageUrl: '/static/logo.png',
      desc: '七都商合 | 示例城市A生活商业服务一站式平台',
    };
  },
};
