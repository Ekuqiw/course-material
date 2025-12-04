// 获取表单、前往相应表单的按钮
const loginForm = document.querySelector('#login-form')
const goRegister = document.querySelector('#go-register')
const registerForm = document.querySelector('#register-form')
const goLogin = document.querySelector('#go-login')

// 绑定点击事件，切换表单
goRegister.addEventListener('click', () => {
    loginForm.style.display = 'none'
    registerForm.style.display = 'block'
})

goLogin.addEventListener('click', () => {
    loginForm.style.display = 'block'
    registerForm.style.display = 'none'
})