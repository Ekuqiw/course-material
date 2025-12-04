// 获取页面元素
// const page = document.querySelector('.page')
// 获取背景图片展示元素
const imgShow = document.querySelector('.img-show')
// 获取所有的图片元素
const imgs = document.querySelectorAll('.img')
// 获取图片列表显示与隐藏元素
const showHidden = document.querySelector('.show-hidden')
// 获取图标
const i = showHidden.querySelector('i')
// 获取滚动容器
const scrollContainer = document.querySelector('.scroll')

// 为图片列表显示与隐藏绑定点击事件
showHidden.addEventListener('click', () => {
    // 图片列表没有显示，则在点击之后进行显示
    if (scrollContainer.classList.contains('hidden')) {
        scrollContainer.classList.remove('hidden')
        // 修改图标
        i.classList.remove('bxs-chevrons-up')
        i.classList.add('bxs-chevrons-down')
    } else {
        scrollContainer.classList.add('hidden')
        // 修改图标
        i.classList.remove('bxs-chevrons-down')
        i.classList.add('bxs-chevrons-up')
    }
})

// 为图片元素绑定点击事件
imgs.forEach(img => {
    // // 点击图片切换页面背景
    // img.addEventListener('click', () => {
    //     page.style.backgroundImage = `url(${img.querySelector('img').src})`
    // })
    // 点击切换背景图片展示的图片
    img.addEventListener('click', () => {
        imgShow.src = img.querySelector('img').src
    })
})