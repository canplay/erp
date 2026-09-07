export default function useUtility() {
  const testPhone = (phone: string) => {
    const reg_tel = /^[1][3,4,5,7,8][0-9]{9}$/;

    if (!reg_tel.test(phone)) {
      return false;
    }
    return true;
  };

  return { testPhone };
}
