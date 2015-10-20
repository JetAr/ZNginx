DirectX 10 Draw Text to texture

//z 2015-10-20 10:30 创建字体以及 sprite 
D3DX10CreateFont(device, 16, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Verdana", &font);
D3DX10CreateSprite(device, 256, &sprite);

hr = sprite->Begin(0);
left = L"Some text to render";
RECT r;
r.top = 0;
r.bottom = window->height;
r.left = 0;
r.right = window->width;
hr = font->DrawTextW(sprite, left.c_str(), -1, &r, DT_LEFT | DT_TOP | DT_CALCRECT, D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f));
hr = font->DrawTextW(sprite, left.c_str(), -1, &r, DT_LEFT | DT_TOP, D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f));
//hr = sprite->Flush();
hr = sprite->End();
device->OMSetBlendState(NULL, 0, 0xffffffff);
