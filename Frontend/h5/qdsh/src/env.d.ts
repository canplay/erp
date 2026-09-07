declare namespace NodeJS {
  interface ProcessEnv {
    NODE_ENV: string
    VUE_ROUTER_MODE: 'hash' | 'history' | 'abstract' | undefined
    VUE_ROUTER_BASE: string | undefined
  }
}

declare interface Window {
  uniWeb: unkonw
  _AMapSecurityConfig: {
    securityJsCode: string
  }
}

declare namespace AMap {
  interface MapOptions {
    center?: LngLat
    zoom?: number
  }

  class Map {
    constructor(div: string | HTMLDivElement, opts: MapOptions)
    destroy(): void
    addControl(control: Control): void
    setZoomAndCenter(zoom: number, center: LngLat, immediately: boolean, duration?: number): void
    panTo(lnglat: LngLat, duration?: number): void
  }

  interface LngLat {
    lng?: number
    lat?: number
    noWrap?: boolean
  }

  interface GeolocationResult {
    position: LngLat
    accuracy: number
    location_type: number
    message: number
    isConverted: number
    info: number
    addressComponent: number
    formattedAddress: number
    pois: number
    roads: number
    crosses: number
  }

  class Geolocation {
    getCurrentPosition(callback: (status: string, result: GeolocationResult) => void): void
  }
}

declare namespace BMapGL {
  class Map {
    constructor(container: string | HTMLElement, opts?: MapOptions)
    centerAndZoom(point: Point, zoom: number): void
    enableScrollWheelZoom(enable?: boolean): void
    addOverlay(overlay: Overlay): void
    removeOverlay(overlay: Overlay): void
    setMapStyleV2(style: { styleJson: Array<Record<string, unkonw>> }): void
    panTo(point: Point): void
    openInfoWindow(info: InfoWindow, point: Point): void
    addCustomHtmlLayer(layer: CustomHtmlLayer): void
    removeCustomHtmlLayer(layer: CustomHtmlLayer): void
    addEventListener(event: string, handler: (e: unkonw) => void): void
    removeEventListener(event: string, handler: (e: unkonw) => void): void
  }

  interface MapOptions {
    enableMapClick?: boolean
    enableIconClick?: boolean
    showControls?: boolean
  }

  class Point {
    constructor(lng: number, lat: number)
    lng: number
    lat: number
  }

  class Label {
    constructor(text: string, opts: { position: Point; offset: Size })
    setStyle(styles: Record<string, string>): void
  }

  class Size {
    constructor(width: number, height: number)
  }

  class InfoWindow {
    constructor(content: string, opts: { width: number; title?: string })
    setMaxContent(content: string): void
    addEventListener(event: string, handler: () => void): void
  }

  class Geolocation {
    getCurrentPosition(callback: (result: unkonw) => void): void
    getStatus(): number
  }

  class CustomHtmlLayer {
    constructor(domElement: HTMLElement | (() => HTMLElement), opts: { point: Point })
    setData(data: unkonw): void
    addEventListener(event: string, handler: () => void): void
  }

  class Overlay {}
}
