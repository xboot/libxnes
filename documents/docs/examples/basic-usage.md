# 基本用法

本示例演示如何使用 libxnes 加载 ROM、运行模拟并显示画面。基于 Linux SDL2 示例。

## 截图

![super-mario](/screenshots/super-mario.png)
![contra](/screenshots/contra.png)
![battle-city](/screenshots/battle-city.png)

## 完整流程

### 1. 加载 ROM 文件

```c
#include <xnes.h>

static void * file_load(const char * filename, uint64_t * len)
{
    FILE * in = fopen(filename, "rb");
    if(in)
    {
        uint64_t offset = 0, bufsize = 8192;
        char * buf = malloc(bufsize);
        while(1)
        {
            uint64_t n = fread(buf + offset, 1, bufsize - offset, in);
            offset += n;
            if(n < bufsize - offset)
                break;
            bufsize *= 2;
            buf = realloc(buf, bufsize);
        }
        if(len)
            *len = offset;
        fclose(in);
        return buf;
    }
    return NULL;
}

void * buf;
uint64_t len;
buf = file_load("game.nes", &len);
struct xnes_ctx_t * nes = xnes_ctx_alloc(buf, len);
free(buf);
```

### 2. 创建状态管理器（用于倒带）

```c
struct xnes_state_t * state = xnes_state_alloc(nes, 60 * 30);
```

### 3. 帧循环

```c
uint64_t timestamp = ktime_get();
uint64_t elapsed = 0;

while(running)
{
    if(ktime_get() - timestamp >= elapsed)
    {
        timestamp = ktime_get();
        elapsed = xnes_step_frame(nes);  // 模拟一帧
        screen_refresh(nes);              // 显示画面
        xnes_state_push(state);           // 保存状态
    }
    else
        SDL_Delay(1);
}
```

### 4. 读取并显示像素

```c
void screen_refresh(struct xnes_ctx_t * nes)
{
    uint32_t * fb = /* 你的 framebuffer */;
    for(int y = 0; y < 240; y++)
    {
        for(int x = 0; x < 256; x++)
        {
            fb[y * 256 + x] = xnes_get_pixel(nes, x, y);
        }
    }
}
```

### 5. 清理

```c
xnes_state_free(state);
xnes_ctx_free(nes);
```

## 更多截图

![flying-hero](/screenshots/flying-hero.png)
![jackal](/screenshots/jackal.png)
![excite-bike](/screenshots/excite-bike.png)
![bomber-man](/screenshots/bomber-man.png)
![duck-hunt](/screenshots/duck-hunt.png)

:::note
完整示例代码参见仓库 `examples/linux/main.c`。
:::
