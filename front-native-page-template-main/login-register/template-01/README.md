> ./css/common.css 中的样式参考(复制)自 windi css；
> 图标库：https://boxicons.com/;
> 矢量图库：https://www.iconfont.cn/

## 页面布局实现思路
1. 页面响应式布局实现中，移动端与 PC 采用 1100px 为分界线，两个布局样式(除了通用共用部分)写在独立的作用域(@media 代码块)中
2. PC 中，使用一个容器对所有的展示元素进行包裹，采用 grip 对容器内空间进行划分，表单放在表单容器中，表单容器实现对表单位置的控制，表单出现动画加在表单上，避免冲突
3. 当宽度小于 1100px 时，所有展示元素的容器撑满屏幕，其内部元素通过 flex 实现居中，至于图片和表单位置偏移，通过缩放和位移实现

> initial 将属性设置为属性的默认值，可以用于将属性恢复到其初始状态。文档：https://developer.mozilla.org/zh-CN/docs/Web/CSS/initial