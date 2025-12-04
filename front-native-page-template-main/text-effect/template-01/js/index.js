// 文本输入与删除的速度(间隔多少毫秒输入/删除一个字符)
const writeSpeed = 100
const delSpeed = 50

// 获取文本元素
const text = document.querySelector('.text')

// 获取文本内容
const textContent = text.innerHTML
const textContentLen = textContent.length

// 初始文本内容为空
text.innerHTML = ''

// 要写入字符的索引
let idx = 0

// 保存定时器
let writeTimer = null
let delTimer = null

// 定时写入字符处理函数
function writing() {
    text.innerHTML += textContent[idx++]
    if (idx >= textContentLen) {
        // 写完清除定时器
        clearInterval(writeTimer)
        // 创建删除文本的定时器
        delTimer = setInterval(deleting, delSpeed)
    }
}

// 定时删除字符处理函数
function deleting() {
    text.innerHTML = textContent.substring(0, idx--)
    if (idx < 0) {
        // 清除删除字符定时器
        clearInterval(delTimer)
        // 进入 if idx = -1，需要对其进行重置
        idx = 0
        // 创建写入字符定时器
        writeTimer = setInterval(writing, writeSpeed)
    }
}

// 定时写入一个字符
writeTimer = setInterval(writing, writeSpeed)



