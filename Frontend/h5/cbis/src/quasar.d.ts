/* eslint-disable */

// Forces TS to apply `@quasar/app-vite` augmentations of `quasar` package
// Removing this would break `quasar/wrappers` imports as those typings are declared
//  into `@quasar/app-vite`
// As a side effect, since `@quasar/app-vite` reference `quasar` to augment it,
//  this declaration also apply `quasar` own
//  augmentations (eg. adds `$q` into Vue component context)
/// <reference types="@quasar/app-vite" />

// BMapGL 地图类型定义
declare const BMapGL: {
  Marker: new (
    position: { lat: number; lng: number },
    options?: {
      enableDragging?: boolean;
      enableMassClearing?: boolean;
      animation?: number;
    }
  ) => {
    getPosition: () => { lat: () => number; lng: () => number };
    setPosition: (position: { lat: number; lng: number }) => void;
    getPositionData: () => { lat: number; lng: number };
    [key: string]: any;
  };
  Label: new (
    content: string,
    options?: {
      position?: { lat: number; lng: number };
      offset?: { width: number; height: number };
      [key: string]: any;
    }
  ) => {
    setContent: (content: string) => void;
    setPosition: (position: { lat: number; lng: number }) => void;
    getPosition: () => { lat: () => number; lng: () => number };
    [key: string]: any;
  };
  Geolocation: new () => {
    getCurrentPosition: (
      callback: (status: string, result: { point: { lat: number; lng: number } }) => void,
      options?: { timeout?: number }
    ) => void;
  };
  Geocoder: new () => {
    getLocation: (
      keyword: string,
      callback: (status: string, result: { point: { lat: number; lng: number } }) => void
    ) => void;
  };
  [key: string]: any;
};

declare const BMAP_STATUS_SUCCESS: number;

// 确保文件作为模块被识别
export {};

declare global {
  interface Window {
    electron: any;
    BMapGL: typeof BMapGL;
    BMAP_STATUS_SUCCESS: typeof BMAP_STATUS_SUCCESS;
  }
}
