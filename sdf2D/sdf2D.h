#pragma once

const char* const HTMLheader = R"(<HTML>
<HEAD>
<META NAME="GENERATOR" Content="Microsoft Visual Studio">
<TITLE></TITLE>
    <style>
        .box {
            display: table;
        }
        .box > div {
            zoom:0.75;
            display: table-cell;
            width: 64px;
            max-width: 64px;
            height: 64px;
            box-sizing: border-box;
            padding: 10px;
            text-align: center;
            vertical-align: middle;
            border: 1px solid black;
        }
    </style>
</HEAD>
<BODY>
    <div class="box">
)";
const char* const HTMLfooter = R"(
    </div>
</BODY>
</HTML>
)";
