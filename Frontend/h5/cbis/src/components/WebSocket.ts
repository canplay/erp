import { Notify } from 'quasar';

// WebSocket 回调函数类型
type WebSocketCallback = (data: unknown) => void;

// WebSocket 发送数据类型
interface WebSocketSendData {
  heartbeat?: number;
  [key: string]: unknown;
}

export default class useWebSocket {
  static instance: useWebSocket | null = null;

  static get Instance() {
    if (useWebSocket.instance) {
      return useWebSocket.instance;
    }
    return (useWebSocket.instance = new useWebSocket());
  }

  ws: WebSocket | null = null;
  callBackMapping: Record<string, WebSocketCallback> = {};
  connected = false;
  sendRetryCount = 0;

  sendFixHeart() {
    let sendFixHeartTimer: ReturnType<typeof setInterval> | null = null;
    if (sendFixHeartTimer) clearInterval(sendFixHeartTimer);

    sendFixHeartTimer = setInterval(() => {
      this.ws!.send(JSON.stringify({ heartbeat: 0 }));
    }, 20000);
  }

  connect(url: string, onMessage: (event: MessageEvent) => void) {
    if (!window.WebSocket) {
      Notify.create({
        message: '客户端不支持消息通知',
        type: 'info',
        color: 'primary',
      });
      return;
    }

    this.ws = new WebSocket('wss://ws.k8s.example.com' + url);

    this.ws.onopen = () => {
      this.connected = true;
      this.sendFixHeart();
    };

    this.ws.onclose = () => {
      this.connected = false;
      setTimeout(() => {
        this.close();
        this.connect(url, onMessage);
      }, 10000);
    };

    this.ws.onerror = () => {
      Notify.create({
        message: '消息通知连接错误',
        type: 'info',
        color: 'primary',
      });
    };

    this.ws.onmessage = onMessage;
  }

  send(data: WebSocketSendData) {
    if (this.connected && this.ws) {
      this.sendRetryCount = 0;
      this.ws.send(JSON.stringify(data));
    } else {
      this.sendRetryCount++;
      setTimeout(() => {
        this.send(data);
        Notify.create({
          message: '第 ' + this.sendRetryCount + ' 次重新获取消息通知',
          type: 'info',
          color: 'primary',
        });
      }, 1000);
    }
  }

  close() {
    this.ws?.close();
  }
}
