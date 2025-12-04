// 获取页面元素
const loginRegisterPage = document.querySelector('.login-register-page')
// 获取前往 登录/注册 按钮
const goLogin = document.querySelector('.go-login')
const goRegister = document.querySelector('.go-register')

// 为按钮绑定事件
goRegister.addEventListener('click', () => {
    loginRegisterPage.classList.add('register-mode')
})
goLogin.addEventListener('click', () => {
    loginRegisterPage.classList.remove('register-mode')
})