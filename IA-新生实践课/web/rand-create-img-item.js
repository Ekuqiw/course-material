const imgItemsContainer = document.querySelector('.img-display-page__img-item-container')
const imgItemTemplate = document.querySelectorAll('.img-item')[0]
const imgItemMaxCnt = 3
const imgItemCreateCntPer = 1
const imgSizeMin = 1080
const imgSizeMax = 1920

const genRandInt = (min, max) => {
    return Math.floor(Math.random() * (max - min + 1) + min)
}

const genRandImgSize = (min, max) => {
    return {
        width: genRandInt(min, max),
        height: genRandInt(min, max)
    }
}


let isInvoke = true

const imgLoading = document.querySelector('.img-display-page__loading')
const notMore = document.querySelector('.img-display-page__not-more')


const genImgItems = () => {

    let imgItemCnt = document.querySelectorAll('.img-item').length

    if (imgItemCnt >= imgItemMaxCnt) {
        notMore.style.display = 'flex'
        return
    } else {
        notMore.style.display = 'none'
    }
    imgLoading.style.display = 'flex'
    let isShowImgCnt = { cnt: 0 }
    const isShowImgCntProxy = new Proxy(isShowImgCnt, {
        set(target, property, value) {
            target[property] = value
            if (isShowImgCnt.cnt === imgItemCreateCntPer) {
                imgLoading.style.display = 'none'
                isInvoke = true
            }
        }
    })
    for (let i = 0; i < imgItemCreateCntPer; i++) {
        let imgItem_clone = imgItemTemplate.cloneNode(true)
        let imgSize = genRandImgSize(imgSizeMin, imgSizeMax)
        let imgSrc = `https://picsum.photos/${imgSize.width}/${imgSize.height}`
        let img = imgItem_clone.querySelector('img')
        img.src = imgSrc
        img.addEventListener('load', () => {
            imgItemsContainer.appendChild(imgItem_clone);
            isShowImgCntProxy.cnt++
        })
        img.addEventListener('error', () => {
            console.log(imgSrc + '图片显示失败');
            img.src = './assets/img-01.jpg'
            imgItemsContainer.appendChild(imgItem_clone);
            isShowImgCntProxy.cnt++
        })
    }
}

const throttle = (handler, delay) => {
    if (isInvoke) {
        isInvoke = false
        handler.apply(this)
    }
}

const debounce2 = (handler, delay) => {
    let timer = null
    return function () {
        if (timer) clearTimeout(timer)
        timer = setTimeout(() => {
            handler.apply(this, arguments)
        }, delay)
    }
}

throttle(genImgItems)

window.addEventListener('scroll', debounce2(() => {
    let docScrollTop = document.body.scrollTop || document.documentElement.scrollTop
    let docVisibleMax = document.documentElement.clientHeight + docScrollTop
    if (docVisibleMax + 200 >= document.body.clientHeight) {

        throttle(genImgItems)
    }
}, 100))