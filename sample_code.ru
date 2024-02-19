kaam calculateFib@adad(num@adad|c@adad)karo
    rakho num := num + 5;
    dekhao << num << `\n`;

    rakho num := num - 5;
    dekhao << num << `\n`;

    rakho num := num * 2;
    dekhao << num << `\n`;

    rakho num := num / 2;
    dekhao << num << `\n`;

    rakho num := num % 2;
    dekhao << num << `\n`;

    rakho i@adad := 0;
    dekhao << `\n`;

    wapas bhaijo 0;
    dekhao << `oh`;
kaam khatam

kaam markazi@adad()karo
    rakho n@adad;
    lo << `Enter the number` >> n;
    dekhao << n << ` `;
    rakho pa@adad:= chalao calculateFib(1 | 2);
    dekhao << pa;

    rakho n := 1;
    agar (n = 1) to phir karo
        dekhao << `aik daal diya\n`;
    warna agar (n = 0) to phir
        dekhao << `yar 0 hai!\n`;
    warna phir
        dekhao << `\nhan ab sahi hay: `;
    bas karo

    wapas bhaijo 0;
    dekhao << `no`;
kaam khatam