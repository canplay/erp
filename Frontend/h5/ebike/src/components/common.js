// 工具函数集 - 用于地图坐标计算
export default {
  /**
   * 计算三点围成的三角形面积
   * @param p0 - 第一个点 [lng, lat]
   * @param p1 - 第二个点 [lng, lat]
   * @param p2 - 第三个点 [lng, lat]
   * @returns 三角形面积
   */
  getArea(p0, p1, p2) {
    let area = 0.0;
    area =
      p0[0] * p1[1] +
      p1[0] * p2[1] +
      p2[0] * p0[1] -
      p1[0] * p0[1] -
      p2[0] * p1[1] -
      p0[0] * p2[1];
    return area / 2;
  },

  /**
   * 计算多边形质心（重心）
   * @param points - 多边形顶点数组 [[lng, lat], ...]
   * @returns 质心坐标 [lng, lat]
   */
  getPolygonCenter(points) {
    if (points.length < 2) return [0, 0];

    let sum_x = 0;
    let sum_y = 0;
    let sum_area = 0;
    let p1 = points[1];

    for (let i = 2; i < points.length; i++) {
      let p2 = points[i];
      let area = this.getArea(points[0], p1, p2);
      sum_area += area;
      sum_x += (points[0][0] + p1[0] + p2[0]) * area;
      sum_y += (points[0][1] + p1[1] + p2[1]) * area;
      p1 = p2;
    }

    let xx = sum_x / sum_area / 3;
    let yy = sum_y / sum_area / 3;
    return [xx, yy];
  },

  /**
   * 判断点是否在多边形内（射线法）
   * @param checkPoint - 待检测点 [lng, lat]
   * @param polygonPoints - 多边形顶点数组
   * @returns 是否在多边形内
   */
  isInPolygon(checkPoint, polygonPoints) {
    let counter = 0;
    let i;
    let xinters;
    let p1, p2;
    let pointCount = polygonPoints.length;
    p1 = polygonPoints[0];

    for (i = 1; i <= pointCount; i++) {
      p2 = polygonPoints[i % pointCount];
      if (
        checkPoint[0] > Math.min(p1[0], p2[0]) &&
        checkPoint[0] <= Math.max(p1[0], p2[0])
      ) {
        if (checkPoint[1] <= Math.max(p1[1], p2[1])) {
          if (p1[0] != p2[0]) {
            xinters =
              ((checkPoint[0] - p1[0]) * (p2[1] - p1[1])) / (p2[0] - p1[0]) +
              p1[1];
            if (p1[1] == p2[1] || checkPoint[1] <= xinters) {
              counter++;
            }
          }
        }
      }
      p1 = p2;
    }
    if (counter % 2 == 0) {
      return false;
    } else {
      return true;
    }
  },
};
