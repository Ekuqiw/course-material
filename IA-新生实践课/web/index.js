const imgItemDisplayClassName = 'display'

const imgDisplayPage = document.querySelector('.img-display-page')

let imgItems = document.querySelectorAll('.img-item')

const changePageBgImgByDisplayImgItem = (imgItem) => {
    imgDisplayPage.style.backgroundImage = `url(${imgItem.querySelector('img').src})`
}

const addDisplay2ImgItemHandler = (idx) => {
    if (idx < 0 || idx >= imgItems.length) return
    let imgItem = imgItems[idx]
    imgItem.classList.add(imgItemDisplayClassName)
    changePageBgImgByDisplayImgItem(imgItem)
}


if (imgItems.length >= 1) {
    addDisplay2ImgItemHandler(0)
}

const docScrollHandler = () => {

    imgItems = document.querySelectorAll('.img-item')
    if (imgItems.length <= 0) return
    let docScrollTop = document.body.scrollTop || document.documentElement.scrollTop
    let docVisibleHeight = document.documentElement.clientHeight

    let docVisibleMin = docScrollTop
    let docVisibleMax = docScrollTop + docVisibleHeight
    let docVisibleMiddle = docVisibleMin + (docVisibleMax - docVisibleMin) / 2

    imgItems.forEach(item => {

        item.classList.remove(imgItemDisplayClassName)

        if (docScrollTop === 0) {
            addDisplay2ImgItemHandler(0)
            return
        }
        if (docVisibleMax === document.body.clientHeight) {
            addDisplay2ImgItemHandler(imgItems.length - 1)
            return
        }
        
        let itemDocMin = item.offsetTop
        let itemDocMax = item.offsetTop + item.offsetHeight

        if (itemDocMin <= docVisibleMiddle && itemDocMax >= docVisibleMiddle) {
            item.classList.add(imgItemDisplayClassName)
            changePageBgImgByDisplayImgItem(item)
        }
    })
}


const debounce = (handler, delay) => {
    let timer = null
    return function () {
        if (timer) clearTimeout(timer)
        timer = setTimeout(() => {
            handler.apply(this, arguments)
        }, delay)
    }
}

document.addEventListener('scroll', debounce(docScrollHandler, 100))

