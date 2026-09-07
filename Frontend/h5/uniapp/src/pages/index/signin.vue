<template>
  <div class="login">
    <form class="form" :modelValue="formData" @submit="onSubmit(true, $event)">
      <button
        class="button"
        open-type="chooseAvatar"
        @chooseavatar="onChooseAvatar"
      >
        <image
          class="image"
          :src="
            formData.avatarUrl
              ? formData.avatarUrl
              : 'https://mmbiz.qpic.cn/mmbiz/icTdbqWNOwNRna42FI242Lcia07jQodd2FJGIYQfG0LAJGFxM4FbnQP6yfMxBgJ0F3YRqJCJ1aPAK2dQagdusBZg/0'
          "
        >
        </image>
        <div class="text">点此选择头像</div>
      </button>

      <div class="loginName">
        <input
          class="input"
          name="nickName"
          type="nickname"
          placeholder="点此输入昵称"
          v-model="formData.nickName"
        />
      </div>

      <div class="loginBtn">
        <button
          class="btn"
          form-type="submit"
          open-type="getUserInfo"
          @getuserinfo="onGetUserInfo"
        >
          一键登录
        </button>
      </div>
    </form>

    <!-- <form class="form" :modelValue="formData" @submit="onSubmit(true, $event)">
      <button
        class="button"
        open-type="chooseAvatar"
        @chooseavatar="onChooseAvatar"
      >
        <image
          class="image"
          :src="
            formData.avatarUrl
              ? formData.avatarUrl
              : 'https://mmbiz.qpic.cn/mmbiz/icTdbqWNOwNRna42FI242Lcia07jQodd2FJGIYQfG0LAJGFxM4FbnQP6yfMxBgJ0F3YRqJCJ1aPAK2dQagdusBZg/0'
          "
        >
        </image>
        <div class="text">点此选择头像</div>
      </button>

      <div class="loginName">
        <input
          class="input"
          name="nickName"
          type="nickname"
          placeholder="点此输入昵称"
          v-model="formData.nickName"
        />
      </div>

      <div class="loginBtn">
        <button
          class="btn"
          form-type="submit"
          open-type="getPhoneNumber"
          @getphonenumber="onGetPhoneNumber"
        >
          <div>立即登录</div>
        </button>
      </div>
    </form> -->
  </div>
</template>

<style lang="scss" scoped>
.login {
  width: 100vw;
  height: 100vh;
  display: flex;
  justify-content: center;
  align-items: center;

  .form {
    display: flex;
    justify-content: center;
    align-items: center;
    flex-direction: column;
    width: 100%;

    .button {
      display: flex;
      flex-direction: column;
      justify-content: center;
      align-items: center;
      background: none;
      color: inherit;
      border: none;
      padding: 0;
      font: inherit;
      cursor: pointer;
      outline: inherit;

      .image {
        height: 240rpx;
        width: 240rpx;
        border-radius: 20rpx;
      }
    }

    .button::after {
      border: none;
    }

    .text {
      font-size: 28rpx;
      color: #cbcbcb;
      margin-top: 20rpx;
    }
  }
}

.loginName {
  display: flex;
  margin: 70rpx 0;
  border: 1px solid #cbcbcb;
  padding: 20rpx 40rpx;
  width: 80vw;
  box-sizing: border-box;
  font-size: 28rpx;
  align-items: center;

  .input {
    margin-right: 40rpx;
  }
}

.loginBtn {
  width: 100%;

  .btn {
    width: 80%;
    border-radius: 20rpx;
    background-color: #1976d2;
    color: #fff;
  }
}
</style>

<script setup lang="ts">
import { ref } from 'vue';
import { useStore } from '@/stores/global';
// import CryptoJS from 'crypto-js';

const store = useStore();

const formData = ref({
  nickName: '',
  avatarUrl: '',
});

const onChooseAvatar = (e: any) => {
  formData.value.avatarUrl = e.detail.avatarUrl;
};

const onSubmit = (confirm: any, event: any) => {
  if (confirm) {
    try {
      let { nickName } = event.detail.value;
      formData.value.nickName = nickName;
    } catch (e: any) {
      uni.showToast({
        title: e.message,
        icon: 'none',
      });
      return;
    }
  }
};

const onGetPhoneNumber = (data: any) => {
  if (
    formData.value.avatarUrl &&
    formData.value.nickName &&
    data.detail.iv &&
    data.detail.encryptedData
  ) {
    uni.showToast({
      title: '登录中...',
      icon: 'loading',
      duration: 1000,
    });

    // this.uploadImage(formData.value.avatarUrl);

    uni.login({
      provider: 'weixin',
      onlyAuthorize: true,
      success: (res) => {
        uni.request({
          url: 'https://gateway-api.ct.example.com/usercenter/token/third/login',
          method: 'POST',
          data: {
            authorize_code: res.code,
            platform: 'WECHAT_MINI_PROGRAM',
            application: 'SHOP_WECHAT_MINI',
            clientId: 'qidu',
          },
          success: (res: any) => {
            if (res.data.status === 'success') {
              store.session = res.data.data;
              uni.redirectTo({
                url: `index?method=${store.session.access_token}`,
              });
            } else {
              uni.showToast({
                title: `登录失败, 请稍后重试 ${JSON.stringify(res)}`,
                icon: 'none',
              });
            }
          },
          fail: (res) => {
            uni.showToast({
              title: `登录失败, 请稍后重试 ${JSON.stringify(res)}`,
              icon: 'none',
            });
          },
        });

        // uni.request({
        //   url: 'https://api.weixin.qq.com/sns/jscode2session', // 请求微信服务器
        //   method: 'GET',
        //   data: {
        //     appid: 'wx0000000000000000',
        //     secret: 'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA',
        //     js_code: res.code,
        //     grant_type: 'authorization_code',
        //   },
        //   success: (res: any) => {
        //     let decrypted = CryptoJS.AES.decrypt(
        //       CryptoJS.format.Hex.parse(
        //         CryptoJS.enc.Base64.parse(data.detail.encryptedData).toString()
        //       ),
        //       CryptoJS.enc.Base64.parse(res.data.session_key),
        //       {
        //         iv: CryptoJS.enc.Base64.parse(data.detail.iv),
        //         mode: CryptoJS.mode.CBC,
        //         padding: CryptoJS.pad.Pkcs7,
        //       }
        //     );

        //     uni.redirectTo({
        //       url: `index?params=${
        //         JSON.parse(decrypted.toString(CryptoJS.enc.Utf8)).phoneNumber
        //       }`,
        //     });
        //     store.login = true;
        //   },
        //   fail(res) {
        //     console.log(`uni.request failed: ${JSON.stringify(res)}`);
        //     uni.showToast({
        //       title: `登录失败, 请稍后重试 ${JSON.stringify(res)}`,
        //       icon: 'none',
        //     });
        //   },
        // });
      },
      fail(res) {
        console.log(`uni.login failed: ${JSON.stringify(res)}`);
        uni.showToast({
          title: `登录失败, 请稍后重试 ${JSON.stringify(res)}`,
          icon: 'none',
        });
      },
    });
  } else {
    uni.showToast({
      title: `请选择头像或输入昵称~`,
      icon: 'none',
    });
  }
};

const onGetUserInfo = (data: any) => {
  try {
    uni.showToast({
      title: '登录中...',
      icon: 'loading',
      duration: 1000,
    });

    uni.login({
      provider: 'weixin',
      onlyAuthorize: true,
      success: (res) => {
        uni.request({
          url: 'https://gateway-api.ct.example.com/usercenter/token/third/login',
          method: 'POST',
          data: {
            authorize_code: res.code,
            platform: 'WECHAT_MINI_PROGRAM',
            application: 'SHOP_WECHAT_MINI',
            clientId: 'qidu',
          },
          success: (res: any) => {
            if (res.data.status === 'success') {
              store.session = res.data.data;
              uni.redirectTo({
                url: `index?method=${store.session.access_token}`,
              });
            } else {
              uni.showToast({
                title: `登录失败, 请稍后重试 ${JSON.stringify(res)}`,
                icon: 'none',
              });
            }
          },
          fail: (res) => {
            uni.showToast({
              title: `登录失败, 请稍后重试 ${JSON.stringify(res)}`,
              icon: 'none',
            });
          },
        });
      },
      fail(res) {
        console.log(`uni.login failed: ${JSON.stringify(res)}`);
        uni.showToast({
          title: `登录失败, 请稍后重试 ${JSON.stringify(res)}`,
          icon: 'none',
        });
      },
    });
  } catch (error) {
    console.error('登录错误: ', error);
    uni.showToast({ title: '登录失败', icon: 'none' });
  }
};
</script>
