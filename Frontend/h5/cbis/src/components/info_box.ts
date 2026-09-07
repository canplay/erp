// BMapGL Overlay类型别名
type BMapOverlay = BMapGL.MapOverlay;

let _content: HTMLElement;

// 信息框属性类型
interface InfoBoxProps {
  _map?: BMapGL.Map;
  _loc?: BMapGL.Point;
  [key: string]: unknown;
}

export default function MyInfoBox(content: HTMLElement) {
  _content = content;
}

// 使用 BMapGL.MapOverlay 接口替代 as 断言
const overlayProto = MyInfoBox.prototype as BMapGL.MapOverlay;

// 使用BMapGL.Overlay的原型对象来继承Overlay的方法
const overlayInstance = Object.create(BMapGL.Overlay.prototype);
Object.keys(overlayInstance).forEach((key) => {
  (overlayProto as unknown as InfoBoxProps)[key] = (overlayInstance as InfoBoxProps)[key];
});

overlayProto.initialize = function (this: BMapOverlay, map: BMapGL.Map) {
  this._map = map;
  this._loc = map.getCenter();
  map.getPanes().floatPane.appendChild(_content);
  return _content;
};

overlayProto.draw = function (this: BMapOverlay) {
  const position = this._map!.pointToOverlayPixel(this._loc!);
  _content.style.left = position.x - _content.clientWidth / 2 + 'px';
  _content.style.top = position.y - _content.clientHeight - 10 + 'px';
};

overlayProto.open = function (this: BMapOverlay, point: BMapGL.Point) {
  this._loc = point;
  this.draw();
  this.show();
};
