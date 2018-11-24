QGraphicsItem::boundingRect() 的作用：
1，他是个纯虚函数
2，定义了当前item的矩形边界，当前item的绘制工作都必须限定在这个区域内
3，QGraphicsView根据此项来决定当前item是否需要重绘。

注意：
如果要改变item的boundingRect，在这之前需要调用prepareGeometryChange().
如果需要花item的轮廓边界，那么boundingRect一定要包含 半个笔宽，左上角 - penWidth/2, 宽高 + penWidth.
