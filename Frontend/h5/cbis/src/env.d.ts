declare namespace NodeJS {
  interface ProcessEnv {
    NODE_ENV: string;
    VUE_ROUTER_MODE: 'hash' | 'history' | 'abstract' | undefined;
    VUE_ROUTER_BASE: string | undefined;
  }
}

// BMapGL (百度地图 WebGL 版本) 类型声明
declare namespace BMapGL {
  // 覆盖物基类
  class Overlay {
    constructor();
    initialize(map: Map): HTMLElement;
    draw(): void;
    show(): void;
    open(point: Point): void;
  }

  // 地图坐标点
  class Point {
    constructor(lng: number, lat: number);
    lng: number;
    lat: number;
  }

  // 地图实例接口
  interface Map {
    getCenter(): Point;
    getPanes(): {
      floatPane: HTMLElement;
    };
    pointToOverlayPixel(point: Point): { x: number; y: number };
  }

  // 地图覆盖物接口
  interface MapOverlay {
    _map?: Map;
    _loc?: Point;
    initialize(map: Map): HTMLElement;
    draw(): void;
    show(): void;
    open(point: Point): void;
  }
}
