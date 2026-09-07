export interface PointType {
  lng: number;
  lat: number;
}

export interface TranslateResult {
  status: number;
  points: PointType[];
}

export interface ConvertorType {
  translate: (
    points: PointType[],
    from: number,
    to: number,
    callback: (result: TranslateResult) => void,
  ) => void;
}

export interface BMapType {
  Point: new (lng: number, lat: number) => PointType;
  Convertor: new () => ConvertorType;
}

export interface MapInstanceType {
  setCenter: (point: PointType) => void;
}

export interface LocationSuccessEvent {
  point: {
    longitude: number;
    latitude: number;
  };
}
