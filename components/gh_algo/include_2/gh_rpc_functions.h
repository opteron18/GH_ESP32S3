//GH3X_GetVersion wrapper BEGIN
#if __SUPPORT_GHEALTH_PROTOCOL__
#pragma pack(1)
typedef struct
{
    uint8_t uchVerType;
} Struct_GH3X_GetVersion;
#pragma pack()

extern void GH3X_GetVersion(uint8_t uchVerType,uint8_t* pchVer,size_t* size);

static void WrapGH3X_GetVersion(Struct_GH3X_GetVersion* point)
{
    uint8_t pchVer[150];
    size_t pchVer_size;
    GH3X_GetVersion(point->uchVerType,pchVer,&pchVer_size);
    GHRPC_return("<u8*>",(RPCPoint){pchVer,pchVer_size});
}

static InvokeNode GH3X_GetVersion_node={
    .key = "GH3X_GetVersion",
    .func = (RpcFunction)WrapGH3X_GetVersion,
    .detail= "<u8>",
    .header = NULL
};
//GH3X_GetVersion wrapper END

//GH3X_RegsWriteCmd wrapper BEGIN
#pragma pack(1)
typedef struct
{
    RPCPoint pusRegs;
} Struct_GH3X_RegsWriteCmd;
#pragma pack()

extern void GH3X_RegsWriteCmd(uint16_t* pusRegs,int32_t nSize);

static void WrapGH3X_RegsWriteCmd(Struct_GH3X_RegsWriteCmd* point)
{
    GH3X_RegsWriteCmd(point->pusRegs.point,point->pusRegs.size);
}

static InvokeNode GH3X_RegsWriteCmd_node={
    .key = "GH3X_RegsWriteCmd",
    .func = (RpcFunction)WrapGH3X_RegsWriteCmd,
    .detail= "<u16*>",
    .header = NULL
};
//GH3X_RegsWriteCmd wrapper END

//GH3X_RegsReadCmd wrapper BEGIN
#pragma pack(1)
typedef struct
{
    uint16_t usRegAddr;
    int32_t nReadLen;
} Struct_GH3X_RegsReadCmd;
#pragma pack()

extern void GH3X_RegsReadCmd(uint16_t usRegAddr,int32_t nReadLen,uint16_t* pusRegValueBuffer,int32_t* pnLen);

static void WrapGH3X_RegsReadCmd(Struct_GH3X_RegsReadCmd* point)
{
    uint16_t pusRegValueBuffer[200];
    int32_t pusRegValueBuffer_size;
    GH3X_RegsReadCmd(point->usRegAddr,point->nReadLen,pusRegValueBuffer,&pusRegValueBuffer_size);
    GHRPC_return("<u16*>",(RPCPoint){pusRegValueBuffer,pusRegValueBuffer_size});
}

static InvokeNode GH3X_RegsReadCmd_node={
    .key = "GH3X_RegsReadCmd",
    .func = (RpcFunction)WrapGH3X_RegsReadCmd,
    .detail= "<u16><d32>",
    .header = NULL
};
//GH3X_RegsReadCmd wrapper END

//GH3X_RegBitFieldWriteCmd wrapper BEGIN
#pragma pack(1)
typedef struct
{
    uint16_t usRegAddr;
    uint8_t uchLsb;
    uint8_t uchMsb;
    uint16_t usRegVal;
} Struct_GH3X_RegBitFieldWriteCmd;
#pragma pack()

extern void GH3X_RegBitFieldWriteCmd(uint16_t usRegAddr,uint8_t uchLsb,uint8_t uchMsb,uint16_t usRegVal);

static void WrapGH3X_RegBitFieldWriteCmd(Struct_GH3X_RegBitFieldWriteCmd* point)
{
    GH3X_RegBitFieldWriteCmd(point->usRegAddr,point->uchLsb,point->uchMsb,point->usRegVal);
}

static InvokeNode GH3X_RegBitFieldWriteCmd_node={
    .key = "GH3X_RegBitFieldWriteCmd",
    .func = (RpcFunction)WrapGH3X_RegBitFieldWriteCmd,
    .detail= "<u16><u8><u8><u16>",
    .header = NULL
};
//GH3X_RegBitFieldWriteCmd wrapper END

//GH3X_ChipCtrl wrapper BEGIN
#pragma pack(1)
typedef struct
{
    uint8_t uchCtrlType;
} Struct_GH3X_ChipCtrl;
#pragma pack()

extern void GH3X_ChipCtrl(uint8_t uchCtrlType);

static void WrapGH3X_ChipCtrl(Struct_GH3X_ChipCtrl* point)
{
    GH3X_ChipCtrl(point->uchCtrlType);
}

static InvokeNode GH3X_ChipCtrl_node={
    .key = "GH3X_ChipCtrl",
    .func = (RpcFunction)WrapGH3X_ChipCtrl,
    .detail= "<u8>",
    .header = NULL
};
//GH3X_ChipCtrl wrapper END

//GH3X_RegsListWriteCmd wrapper BEGIN
#pragma pack(1)
typedef struct
{
    RPCPoint usRegs;
} Struct_GH3X_RegsListWriteCmd;
#pragma pack()

extern void GH3X_RegsListWriteCmd(uint16_t* usRegs,uint16_t usLen);

static void WrapGH3X_RegsListWriteCmd(Struct_GH3X_RegsListWriteCmd* point)
{
    GH3X_RegsListWriteCmd(point->usRegs.point,point->usRegs.size);
}

static InvokeNode GH3X_RegsListWriteCmd_node={
    .key = "GH3X_RegsListWriteCmd",
    .func = (RpcFunction)WrapGH3X_RegsListWriteCmd,
    .detail= "<u16*>",
    .header = NULL
};
//GH3X_RegsListWriteCmd wrapper END

//GH3X_SwFunctionCmd wrapper BEGIN
#pragma pack(1)
typedef struct
{
    uint32_t unTargetFuncMode;
    uint8_t uchCtrlType;
} Struct_GH3X_SwFunctionCmd;
#pragma pack()

extern void GH3X_SwFunctionCmd(uint32_t unTargetFuncMode,uint8_t uchCtrlType);

static void WrapGH3X_SwFunctionCmd(Struct_GH3X_SwFunctionCmd* point)
{
    GH3X_SwFunctionCmd(point->unTargetFuncMode,point->uchCtrlType);
}

static InvokeNode GH3X_SwFunctionCmd_node={
    .key = "GH3X_SwFunctionCmd",
    .func = (RpcFunction)WrapGH3X_SwFunctionCmd,
    .detail= "<u32><u8>",
    .header = NULL
};
//GH3X_SwFunctionCmd wrapper END

//GH3X_FwUpdateCmd wrapper BEGIN
#pragma pack(1)
typedef struct
{
    RPCPoint pSrc;
} Struct_GH3X_FwUpdateCmd;
#pragma pack()

extern void GH3X_FwUpdateCmd(uint8_t* pSrc,uint32_t usLen,uint8_t* puchRet,uint32_t* pRetLen);

static void WrapGH3X_FwUpdateCmd(Struct_GH3X_FwUpdateCmd* point)
{
    uint8_t puchRet[100];
    uint32_t puchRet_size;
    GH3X_FwUpdateCmd(point->pSrc.point,point->pSrc.size,puchRet,&puchRet_size);
    GHRPC_return("<u8*>",(RPCPoint){puchRet,puchRet_size});
}

static InvokeNode GH3X_FwUpdateCmd_node={
    .key = "FW",
    .func = (RpcFunction)WrapGH3X_FwUpdateCmd,
    .detail= "<u8*>",
    .header = NULL
};
//GH3X_FwUpdateCmd wrapper END

//GH3X_RegsBitFieldWriteCmd wrapper BEGIN
#pragma pack(1)
typedef struct
{
    RPCPoint usRegBits;
} Struct_GH3X_RegsBitFieldWriteCmd;
#pragma pack()

extern void GH3X_RegsBitFieldWriteCmd(uint16_t* usRegBits,size_t size);

static void WrapGH3X_RegsBitFieldWriteCmd(Struct_GH3X_RegsBitFieldWriteCmd* point)
{
    GH3X_RegsBitFieldWriteCmd(point->usRegBits.point,point->usRegBits.size);
}

static InvokeNode GH3X_RegsBitFieldWriteCmd_node={
    .key = "GH3X_RegsBitFieldWriteCmd",
    .func = (RpcFunction)WrapGH3X_RegsBitFieldWriteCmd,
    .detail= "<u16*>",
    .header = NULL
};
//GH3X_RegsBitFieldWriteCmd wrapper END

//GHSetWorkModeCmd wrapper BEGIN
#pragma pack(1)
typedef struct
{
    uint8_t uchWorkMode;
} Struct_GHSetWorkModeCmd;
#pragma pack()

extern void GHSetWorkModeCmd(uint8_t uchWorkMode);

static void WrapGHSetWorkModeCmd(Struct_GHSetWorkModeCmd* point)
{
    GHSetWorkModeCmd(point->uchWorkMode);
}

static InvokeNode GHSetWorkModeCmd_node={
    .key = "GHSetWorkModeCmd",
    .func = (RpcFunction)WrapGHSetWorkModeCmd,
    .detail= "<u8>",
    .header = NULL
};
#endif
//GHSetWorkModeCmd wrapper END

InvokeNode* gh_static_nodes[] = {
#if __SUPPORT_GHEALTH_PROTOCOL__
    &GH3X_FwUpdateCmd_node, &GH3X_ChipCtrl_node, &GH3X_GetVersion_node, &GH3X_RegBitFieldWriteCmd_node, &GH3X_RegsBitFieldWriteCmd_node, &GH3X_RegsListWriteCmd_node, &GH3X_RegsReadCmd_node, &GH3X_RegsWriteCmd_node, &GH3X_SwFunctionCmd_node, &GHSetWorkModeCmd_node
#else
    0
#endif
};
