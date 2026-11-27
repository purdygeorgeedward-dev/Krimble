typedef enum
{
  GIMP_NORMAL_MODE,
  GIMP_DISSOLVE_MODE,
  GIMP_BEHIND_MODE,
  GIMP_MULTIPLY_MODE,
  GIMP_SCREEN_MODE,
  GIMP_OVERLAY_MODE,
  GIMP_DIFFERENCE_MODE,
  GIMP_ADDITION_MODE,
  GIMP_SUBTRACT_MODE,
  GIMP_DARKEN_ONLY_MODE,
  GIMP_LIGHTEN_ONLY_MODE,
  GIMP_HUE_MODE,
  GIMP_SATURATION_MODE,
  GIMP_COLOR_MODE,
  GIMP_VALUE_MODE,
  GIMP_DIVIDE_MODE,
  GIMP_DODGE_MODE,
  GIMP_BURN_MODE,
  GIMP_HARDLIGHT_MODE,
  GIMP_SOFTLIGHT_MODE,
  GIMP_GRAIN_EXTRACT_MODE,
  GIMP_GRAIN_MERGE_MODE,
  GIMP_COLOR_ERASE_MODE,
  GIMP_ERASE_MODE,
  GIMP_REPLACE_MODE,
  GIMP_ANTI_ERASE_MODE
  ,GIMP_NORMAL_NOPARTIAL_MODE=-1
} GimpLayerModeEffects;
const char *showGimpLayerModeEffects(GimpLayerModeEffects);
#define GimpLayerModeEffects_LAST GIMP_ANTI_ERASE_MODE
typedef enum
{
  GIMP_RGB,
  GIMP_GRAY,
  GIMP_INDEXED
} GimpImageBaseType;
const char *showGimpImageBaseType(GimpImageBaseType);
#define GimpImageBaseType_LAST GIMP_INDEXED
typedef enum
{
  GIMP_RGB_IMAGE,
  GIMP_RGBA_IMAGE,
  GIMP_GRAY_IMAGE,
  GIMP_GRAYA_IMAGE,
  GIMP_INDEXED_IMAGE,
  GIMP_INDEXEDA_IMAGE
} GimpImageType;
const char *showGimpImageType(GimpImageType);
#define GimpImageType_LAST GIMP_INDEXEDA_IMAGE
typedef enum
{
  PROP_END                =  0,
  PROP_COLORMAP           =  1,
  PROP_ACTIVE_LAYER       =  2,
  PROP_ACTIVE_CHANNEL     =  3,
  PROP_SELECTION          =  4,
  PROP_FLOATING_SELECTION =  5,
  PROP_OPACITY            =  6,
  PROP_MODE               =  7,
  PROP_VISIBLE            =  8,
  PROP_LINKED             =  9,
  PROP_LOCK_ALPHA         = 10,
  PROP_APPLY_MASK         = 11,
  PROP_EDIT_MASK          = 12,
  PROP_SHOW_MASK          = 13,
  PROP_SHOW_MASKED        = 14,
  PROP_OFFSETS            = 15,
  PROP_COLOR              = 16,
  PROP_COMPRESSION        = 17,
  PROP_GUIDES             = 18,
  PROP_RESOLUTION         = 19,
  PROP_TATTOO             = 20,
  PROP_PARASITES          = 21,
  PROP_UNIT               = 22,
  PROP_PATHS              = 23,
  PROP_USER_UNIT          = 24,
  PROP_VECTORS            = 25,
  PROP_TEXT_LAYER_FLAGS   = 26,
  PROP_SAMPLE_POINTS      = 27,
  PROP_LOCK_CONTENT       = 28,
  PROP_GROUP_ITEM         = 29,
  PROP_ITEM_PATH          = 30,
  PROP_GROUP_ITEM_FLAGS   = 31
} PropType;
const char *showPropType(PropType);
#define PropType_LAST PROP_GROUP_ITEM_FLAGS
typedef enum
{
  COMPRESS_NONE              =  0,
  COMPRESS_RLE               =  1,
  COMPRESS_ZLIB              =  2,
  COMPRESS_FRACTAL           =  3
} XcfCompressionType;
const char *showXcfCompressionType(XcfCompressionType);
#define XcfCompressionType_LAST COMPRESS_FRACTAL
