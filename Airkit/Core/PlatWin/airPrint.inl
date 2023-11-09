
inline void Print::print(int32_t clr, cstring fmt, ...)
{
    HANDLE hCns = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hCns, clr);
    va_list ap;
    va_start(ap, fmt);
    auto cnt = vsnprintf_s(mBuffer, sizeof(mBuffer), fmt, ap);
    va_end(ap);
    puts(mBuffer);
    SetConsoleTextAttribute(hCns, White);
}