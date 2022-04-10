#include "vulkan_renderer.h"
#include <algorithm>

vulkan_renderer::vulkan_renderer(SDL_Window* Window_, u32 Width_, u32 Height_)
{
    Width  = Width_;
    Height = Height_;
    Window = Window_;

    ImageIndex = 0;
    Swapchain = 0;
}

VkBool32 DebugReportCallback(VkDebugReportFlagsEXT Flags, VkDebugReportObjectTypeEXT ObjectType, 
                             uint64_t Object, size_t Location, int32_t MessageCode, 
                             const char* pLayerPrefix, const char* pMessage, void* UserData)
{
    const char* Type = ((Flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) ? 
            "Warning" : ((Flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) ? 
            "Performance Warning" : ((Flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) ? "Error" : "Other")));

    char Message[2048];
    sprintf_s(Message, 2047, "Error Type: %s / %s\n", Type, pMessage);
    printf("%s", Message);

    return VK_FALSE;
}

void vulkan_renderer::
InitVulkanRenderer()
{
    // Create Instance of the vulkan program
    VkApplicationInfo AppInfo = {};
    AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    AppInfo.apiVersion = VK_API_VERSION_1_2;

    VkInstanceCreateInfo InstanceCreateInfo = {};
    InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    InstanceCreateInfo.pApplicationInfo = &AppInfo;

#if CHESS_DEBUG
    std::vector<const char*> ValidationLayers;
    ValidationLayers.push_back("VK_LAYER_KHRONOS_validation");

    InstanceCreateInfo.ppEnabledLayerNames = ValidationLayers.data();
    InstanceCreateInfo.enabledLayerCount = (u32)ValidationLayers.size();
#endif

    std::vector<const char*> Extensions;
    Extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#if defined(VK_USE_PLATFORM_WIN32_KHR)
    Extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif
#if CHESS_DEBUG
    Extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif

    InstanceCreateInfo.ppEnabledExtensionNames = Extensions.data();
    InstanceCreateInfo.enabledExtensionCount = (u32)Extensions.size();

    VK_CHECK(vkCreateInstance(&InstanceCreateInfo, nullptr, &this->Instance));

    PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(Instance, "vkCreateDebugReportCallbackEXT");

    VkDebugReportCallbackCreateInfoEXT DebugCallbackCreateInfo = {VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT};
    DebugCallbackCreateInfo.pfnCallback = DebugReportCallback;
    DebugCallbackCreateInfo.flags = VK_DEBUG_REPORT_WARNING_BIT_EXT | 
                                    VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
                                    VK_DEBUG_REPORT_ERROR_BIT_EXT;

    DebugCallback = 0;
    vkCreateDebugReportCallbackEXT(Instance, &DebugCallbackCreateInfo, 0, &DebugCallback);

    // Get all physical devices
    u32 DeviceCount;
    vkEnumeratePhysicalDevices(Instance, &DeviceCount, nullptr);
    std::vector<VkPhysicalDevice> PhysicalDevices(DeviceCount);
    vkEnumeratePhysicalDevices(Instance, &DeviceCount, PhysicalDevices.data());

    // Pick physical device
    // Picking first disrete gpu
    // Maybe picking gpu by user, but for that I should do another function
    for(VkPhysicalDevice PhysDevice_ : PhysicalDevices)
    {
        VkPhysicalDeviceProperties PhysicalDeviceProperties = {};
        vkGetPhysicalDeviceProperties(PhysDevice_, &PhysicalDeviceProperties);

        if(PhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            PhysicalDevice = PhysDevice_;
            break;
        }
    }

    // Create Device
    VkPhysicalDeviceFeatures DeviceFeatures;
    vkGetPhysicalDeviceFeatures(PhysicalDevice, &DeviceFeatures);
    VkPhysicalDeviceProperties DeviceProperties;
    vkGetPhysicalDeviceProperties(PhysicalDevice, &DeviceProperties);

#if 0
    u32 ExtensionsCount;
    vkEnumerateInstanceExtensionProperties(nullptr, &ExtensionsCount, nullptr);
    std::vector<VkExtensionProperties> AvailableExtensions(ExtensionsCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &ExtensionsCount, AvailableExtensions.data());
#endif

    u32 QueueFamiliesCount;
    vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamiliesCount, nullptr);
    std::vector<VkQueueFamilyProperties> QueueFamilies(QueueFamiliesCount);
    vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamiliesCount, QueueFamilies.data());

    QueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    VkQueueFlags DesiredQueueCapabilities = VK_QUEUE_GRAPHICS_BIT;
    for(u32 Index = 0;
        Index < QueueFamiliesCount;
        ++Index)
    {
        VkQueueFamilyProperties QueueFamilyProperties_ = QueueFamilies[Index];
        if((QueueFamilyProperties_.queueCount > 0) && (QueueFamilyProperties_.queueFlags & DesiredQueueCapabilities))
        {
            QueueFamilyIndex = Index;
        }
    }

    std::vector<float> QueuePriorities;
    QueuePriorities.push_back(1.0f);

    VkDeviceQueueCreateInfo DeviceQueueCreateInfo = {};
    DeviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    DeviceQueueCreateInfo.queueCount = (u32)QueuePriorities.size();
    DeviceQueueCreateInfo.pQueuePriorities = QueuePriorities.data();

    std::vector<const char*> DeviceExtensions;
    DeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    DeviceExtensions.push_back(VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME);

    VkDeviceCreateInfo DeviceCreateInfo = {};
    DeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    DeviceCreateInfo.queueCreateInfoCount = 1;
    DeviceCreateInfo.pQueueCreateInfos = &DeviceQueueCreateInfo;
    DeviceCreateInfo.enabledExtensionCount = (u32)DeviceExtensions.size();
    DeviceCreateInfo.ppEnabledExtensionNames = DeviceExtensions.data();

    vkCreateDevice(PhysicalDevice, &DeviceCreateInfo, 0, &LogicalDevice);

    vkGetPhysicalDeviceMemoryProperties(PhysicalDevice, &MemProperty);
    vkGetDeviceQueue(LogicalDevice, QueueFamilyIndex, 0, &Queue);

    // Create Surface
    VkWin32SurfaceCreateInfoKHR SurfaceCreateInfo = {};
    SurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;

#if defined(VK_USE_PLATFORM_WIN32_KHR)
    SDL_SysWMinfo WindowInfo;
    SDL_VERSION(&WindowInfo.version);
    SDL_GetWindowWMInfo(Window, &WindowInfo);

    SurfaceCreateInfo.hinstance = WindowInfo.info.win.hinstance;
    SurfaceCreateInfo.hwnd = WindowInfo.info.win.window;
#endif

    vkCreateWin32SurfaceKHR(Instance, &SurfaceCreateInfo, 0, &Surface);

    VkBool32 PresentSupported = VK_FALSE;
    vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice, QueueFamilyIndex, Surface, &PresentSupported);

    VkPresentModeKHR DesiredPresentMode;

    u32 PresentModesCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, Surface, &PresentModesCount, nullptr);
    std::vector<VkPresentModeKHR> PresentModes(PresentModesCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, Surface, &PresentModesCount, PresentModes.data());

    DesiredPresentMode = PresentModes[PresentModes.size() - 1];

    u32 SurfaceFormatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface, &SurfaceFormatCount, nullptr);
    std::vector<VkSurfaceFormatKHR> SurfaceFormats(SurfaceFormatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface, &SurfaceFormatCount, SurfaceFormats.data());

    for(u32 SurfaceFormatIndex = 0;
        SurfaceFormatIndex < SurfaceFormatCount;
        ++SurfaceFormatIndex)
    {
        VkSurfaceFormatKHR SurfaceFormat_ = SurfaceFormats[SurfaceFormatIndex];
        if(SurfaceFormat_.format == VK_FORMAT_B8G8R8A8_UNORM)
        {
            SwapchainSurfaceFormat = SurfaceFormat_;
            break;
        }
    }

    CreateSwapchain();

    AcquireSemaphore = CreateSemaphore();
    ReleaseSemaphore = CreateSemaphore();
    Fence = CreateFence();

    VkCommandPoolCreateInfo CommandPoolCreateInfo = {};
    CommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    CommandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT|VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    CommandPoolCreateInfo.queueFamilyIndex = QueueFamilyIndex;

    vkCreateCommandPool(LogicalDevice, &CommandPoolCreateInfo, 0, &CommandPool);

    VkCommandBufferAllocateInfo CommandBufferAllocateInfo = {};
    CommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    CommandBufferAllocateInfo.commandPool = CommandPool;
    CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    CommandBufferAllocateInfo.commandBufferCount = 1;

    vkAllocateCommandBuffers(LogicalDevice, &CommandBufferAllocateInfo, &CommandBuffer);
}

void
vulkan_renderer::InitGraphicsPipeline()
{
}

void
vulkan_renderer::CreateSwapchain(u32 WindowWidth_, u32 WindowHeight_)
{
    if(!WindowWidth_)
    {
        WindowWidth_ = Width;
    }
    
    if(!WindowHeight_)
    {
        WindowHeight_ = Height;
    }

    VkSurfaceCapabilitiesKHR SurfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysicalDevice, Surface, &SurfaceCapabilities);

    VkCompositeAlphaFlagBitsKHR SurfaceComposite = 
        (SurfaceCapabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR) 
        ? VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR : 
        (SurfaceCapabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR) 
        ? VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR : 
        (SurfaceCapabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR) 
        ? VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR
        : VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;

    VkSwapchainCreateInfoKHR SwapchainCreateInfo = {};
    SwapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    SwapchainCreateInfo.surface = Surface;
    SwapchainCreateInfo.minImageCount = Max(2, SurfaceCapabilities.minImageCount);
    SwapchainCreateInfo.imageFormat = SwapchainSurfaceFormat.format;
    SwapchainCreateInfo.imageUsage  = SurfaceCapabilities.supportedUsageFlags;
    SwapchainCreateInfo.imageColorSpace = SwapchainSurfaceFormat.colorSpace;
    SwapchainCreateInfo.imageExtent.width = WindowWidth_;
    SwapchainCreateInfo.imageExtent.height = WindowHeight_;
    SwapchainCreateInfo.imageArrayLayers = 1;
    //SwapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    SwapchainCreateInfo.queueFamilyIndexCount = 1;
    SwapchainCreateInfo.pQueueFamilyIndices = &QueueFamilyIndex;
    SwapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR; //SurfaceCapabilities.currentTransform;
    SwapchainCreateInfo.compositeAlpha = SurfaceComposite;
    SwapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;

    if(Swapchain)
    {
        SwapchainCreateInfo.oldSwapchain = Swapchain;
    }

    VK_CHECK(vkCreateSwapchainKHR(LogicalDevice, &SwapchainCreateInfo, 0, &Swapchain));

    u32 ImagesCount;
    vkGetSwapchainImagesKHR(LogicalDevice, Swapchain, &ImagesCount, nullptr);
    SwapchainImages.resize(ImagesCount);
    vkGetSwapchainImagesKHR(LogicalDevice, Swapchain, &ImagesCount, SwapchainImages.data());

    for(u32 ImageViewIndex = 0;
        ImageViewIndex < ImagesCount;
        ++ImageViewIndex)
    {
        VkImageView ImageView = CreateImageView(SwapchainImages[ImageViewIndex]);
        SwapchainImageViews.push_back(ImageView);
    }

    CreateRenderPass();

    for(u32 FramebufferIndex = 0;
        FramebufferIndex < ImagesCount;
        ++FramebufferIndex)
    {
        VkFramebuffer Framebuffer_ = CreateFramebuffer(SwapchainImageViews[FramebufferIndex]);
        SwapchainFramebuffers.push_back(Framebuffer_);
    }
}

void vulkan_renderer::
DestroySwapchain()
{
    if(Swapchain != VK_NULL_HANDLE)
    {
        vkDestroySwapchainKHR(LogicalDevice, Swapchain, nullptr);
        Swapchain = VK_NULL_HANDLE;
    }
}

VkSemaphore vulkan_renderer::
CreateSemaphore()
{
    VkSemaphoreCreateInfo SemaphoreCreateInfo = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    
    VkSemaphore ResultSemaphore = 0;
    VK_CHECK(vkCreateSemaphore(LogicalDevice, &SemaphoreCreateInfo, 0, &ResultSemaphore));
    return ResultSemaphore;
}

VkFence vulkan_renderer::
CreateFence()
{
    VkFenceCreateInfo FenceCreateInfo = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};

    VkFence ResultFence = 0;
    VK_CHECK(vkCreateFence(LogicalDevice, &FenceCreateInfo, 0, &ResultFence));
    return ResultFence;
}

buffer vulkan_renderer::
AllocateBuffer(size_t Size, VkBufferUsageFlags BufferUsage, VkMemoryPropertyFlags MemoryFlags)
{
    buffer Result = {};
    Result.Size = Size;
    
    VkBufferCreateInfo BufferCreateInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    BufferCreateInfo.size = Size;
    BufferCreateInfo.usage = BufferUsage;
    BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    vkCreateBuffer(LogicalDevice, &BufferCreateInfo, nullptr, &Result.Buffer);

    VkMemoryRequirements MemoryRequirements = {};
    vkGetBufferMemoryRequirements(LogicalDevice, Result.Buffer, &MemoryRequirements);

    u32 MemoryType = ~0u;
    for(u32 Type = 0;
        Type < MemProperty.memoryTypeCount;
        ++Type)
    {
        if((MemoryRequirements.memoryTypeBits & (1 << Type)) && ((MemProperty.memoryTypes[Type].propertyFlags & MemoryFlags) == MemoryFlags))
        {
            MemoryType = Type;
            break;
        }
    }

    VkMemoryAllocateInfo BufferMemoryAllocateInfo = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    BufferMemoryAllocateInfo.allocationSize  = Size;
    BufferMemoryAllocateInfo.memoryTypeIndex = MemoryType;

    vkAllocateMemory(LogicalDevice, &BufferMemoryAllocateInfo, nullptr, &Result.Memory);
    vkBindBufferMemory(LogicalDevice, Result.Buffer, Result.Memory, 0);

    if(MemoryFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
    {
        vkMapMemory(LogicalDevice, Result.Memory, 0, Size, 0, &Result.Data);
    }

    return Result;
}

void vulkan_renderer::
UpdateBuffer(buffer& Buffer, buffer& Scratch, void* Data, size_t Size)
{
    memcpy(Scratch.Data, Data, Size);

    BeginCommands();

    VkBufferCopy CopyOffset = {0, 0, (VkDeviceSize)Size};
    vkCmdCopyBuffer(CommandBuffer, Scratch.Buffer, Buffer.Buffer, 1, &CopyOffset);

#if 1
    VkBufferMemoryBarrier MemoryBarrier = CreateMemoryBarrier(Buffer, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT);
    vkCmdPipelineBarrier(CommandBuffer, 
                         VK_PIPELINE_STAGE_TRANSFER_BIT, 
                         VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 
                         VK_DEPENDENCY_BY_REGION_BIT, 
                         0, 0, 1, &MemoryBarrier, 0, 0);
#endif

    EndCommands();
}

VkBufferMemoryBarrier vulkan_renderer::
CreateMemoryBarrier(buffer& Buffer, VkAccessFlags OldAccess, VkAccessFlags NewAccess)
{
    VkBufferMemoryBarrier MemoryBarrier = {VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER};

    MemoryBarrier.srcAccessMask = OldAccess;
    MemoryBarrier.dstAccessMask = NewAccess;
    MemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    MemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    MemoryBarrier.buffer = Buffer.Buffer;
    MemoryBarrier.offset = 0;
    MemoryBarrier.size   = Buffer.Size;

    return MemoryBarrier;
}

VkImageMemoryBarrier vulkan_renderer::
CreateImageBarrier(image& Image, VkAccessFlags OldAccess, VkAccessFlags NewAccess, VkImageLayout OldLayout, VkImageLayout NewLayout)
{
    VkImageMemoryBarrier ImageBarrier = {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};

    ImageBarrier.srcAccessMask = OldAccess;
    ImageBarrier.dstAccessMask = NewAccess;
    ImageBarrier.oldLayout = OldLayout;
    ImageBarrier.newLayout = NewLayout;
    ImageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    ImageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    ImageBarrier.image = Image.Image;
    ImageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    ImageBarrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
    ImageBarrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

    return ImageBarrier;
}

image vulkan_renderer::
CreateImage(u32 ImageWidth, u32 ImageHeight, VkImageUsageFlags Usage, VkMemoryPropertyFlags MemoryFlags, u32 LayersCount, VkBool32 ShouldBeCubemap)
{
    image Result  = {};
    Result.Width  = ImageWidth;
    Result.Height = ImageHeight;

    VkImageCreateInfo ImageCreateInfo = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
    ImageCreateInfo.flags = ShouldBeCubemap ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;
    ImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    ImageCreateInfo.format = SwapchainSurfaceFormat.format;
    ImageCreateInfo.extent.width = ImageWidth;
    ImageCreateInfo.extent.height = ImageHeight;
    ImageCreateInfo.extent.depth = 1;
    ImageCreateInfo.mipLevels = 1;
    ImageCreateInfo.arrayLayers = ShouldBeCubemap ? 6 * LayersCount : LayersCount;
    ImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    ImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    ImageCreateInfo.usage = Usage;
    ImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    ImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    vkCreateImage(LogicalDevice, &ImageCreateInfo, 0, &Result.Image);

    VkMemoryRequirements MemoryRequirements = {};
    vkGetImageMemoryRequirements(LogicalDevice, Result.Image, &MemoryRequirements);

    u32 MemoryType = ~0u;
    for(u32 Type = 0;
        Type < MemProperty.memoryTypeCount;
        ++Type)
    {
        if((MemoryRequirements.memoryTypeBits & (1 << Type)) && ((MemProperty.memoryTypes[Type].propertyFlags & MemoryFlags) == MemoryFlags))
        {
            MemoryType = Type;
            break;
        }
    }

    VkMemoryAllocateInfo ImageAllocateInfo = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    ImageAllocateInfo.allocationSize  = MemoryRequirements.size;
    ImageAllocateInfo.memoryTypeIndex = MemoryType;
    vkAllocateMemory(LogicalDevice, &ImageAllocateInfo, nullptr, &Result.Memory);

    vkBindImageMemory(LogicalDevice, Result.Image, Result.Memory, 0);

    Result.View = CreateImageView(Result.Image);

    return Result;
}

void
vulkan_renderer::UpdateTexture(image& Image, buffer& Scratch)
{
    //BeginCommands();
    VkBufferImageCopy BufferImageCopy = {};
    BufferImageCopy.bufferOffset = 0;
    BufferImageCopy.bufferRowLength = Image.Width;
    BufferImageCopy.bufferImageHeight = Image.Height;
    BufferImageCopy.imageSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
    BufferImageCopy.imageOffset = {0, 0, 0};
    BufferImageCopy.imageExtent = {Image.Width, Image.Height, 1};

    vkCmdCopyBufferToImage(CommandBuffer, Scratch.Buffer, Image.Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 1, &BufferImageCopy);
    //EndCommands();
}

VkImageView
vulkan_renderer::CreateImageView(VkImage Image)
{
    VkImageSubresourceRange Range = {};
    Range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    Range.levelCount = 1;
    Range.layerCount = 1;

    VkImageViewCreateInfo ImageViewCreateInfo = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    ImageViewCreateInfo.image = Image;
    ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    ImageViewCreateInfo.format = SwapchainSurfaceFormat.format;
    ImageViewCreateInfo.subresourceRange = Range;

    VkImageView ImageViewResult = 0;
    VK_CHECK(vkCreateImageView(LogicalDevice, &ImageViewCreateInfo, 0, &ImageViewResult));

    return ImageViewResult;
}

VkFramebuffer
vulkan_renderer::CreateFramebuffer(VkImageView ImageView_)
{
    VkFramebufferCreateInfo FbCreateInfo = {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
    FbCreateInfo.renderPass = RenderPass;
    FbCreateInfo.attachmentCount = 1;
    FbCreateInfo.pAttachments = &ImageView_;
    FbCreateInfo.width = Width;
    FbCreateInfo.height = Height;
    FbCreateInfo.layers = 1;

    VkFramebuffer FramebufferResult;
    VK_CHECK(vkCreateFramebuffer(LogicalDevice, &FbCreateInfo, 0, &FramebufferResult));
    return FramebufferResult;
}

VkSampler
vulkan_renderer::CreateSampler(VkFilter Filter, VkSamplerAddressMode AddressMode)
{
    VkSamplerCreateInfo SamplerCreateInfo = {VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};
    SamplerCreateInfo.magFilter = Filter;
    SamplerCreateInfo.minFilter = Filter;
    SamplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
    SamplerCreateInfo.addressModeU = AddressMode;
    SamplerCreateInfo.addressModeV = AddressMode;
    SamplerCreateInfo.addressModeW = AddressMode;
    SamplerCreateInfo.unnormalizedCoordinates = false;

    VkSampler SamplerResult;
    VK_CHECK(vkCreateSampler(LogicalDevice, &SamplerCreateInfo, 0, &SamplerResult));
    return SamplerResult;
}

VkDescriptorSetLayout
vulkan_renderer::CreateDescriptorSetLayout()
{
    std::vector<VkDescriptorSetLayoutBinding> Bindings(2);
    Bindings[0].binding = 0;
    Bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    Bindings[0].descriptorCount = 1;
    Bindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    Bindings[1].binding = 1;
    Bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    Bindings[1].descriptorCount = 1;
    Bindings[1].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
    DescriptorSetLayoutCreateInfo.pBindings = Bindings.data();
    DescriptorSetLayoutCreateInfo.bindingCount = (u32)Bindings.size();

    VkDescriptorSetLayout DescriptorSetLayoutResult;
    VK_CHECK(vkCreateDescriptorSetLayout(LogicalDevice, &DescriptorSetLayoutCreateInfo, 0, &DescriptorSetLayoutResult));

    return DescriptorSetLayoutResult;
}

void
vulkan_renderer::CreateRenderPass()
{
    VkAttachmentDescription Attachment = {};
    Attachment.format = SwapchainSurfaceFormat.format;
    Attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    Attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    Attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    Attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    Attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    Attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    Attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference AttachmentReference;
    AttachmentReference.attachment = 0;
    AttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription Subpass = {};
    Subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    Subpass.colorAttachmentCount = 1;
    Subpass.pColorAttachments = &AttachmentReference;

    VkRenderPassCreateInfo RenderPassCreateInfo = {VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
    RenderPassCreateInfo.attachmentCount = 1;
    RenderPassCreateInfo.pAttachments = &Attachment;
    RenderPassCreateInfo.subpassCount = 1;
    RenderPassCreateInfo.pSubpasses = &Subpass;

    VK_CHECK(vkCreateRenderPass(LogicalDevice, &RenderPassCreateInfo, 0, &RenderPass));
}

void vulkan_renderer::
BeginCommands()
{
    // NOTE: I could create command buffer and command pool here if I want to.
    // Any advantages, if so?
    VkCommandBufferBeginInfo CommandBufferBeginInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(CommandBuffer, &CommandBufferBeginInfo);
}

void vulkan_renderer::
EndCommands(VkSemaphore* AcquireSemaphore_, VkSemaphore* ReleaseSemaphore_)
{
    vkEndCommandBuffer(CommandBuffer);

    VkPipelineStageFlags SubmitStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo SubmitInfo = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
    SubmitInfo.commandBufferCount = 1;
    SubmitInfo.pCommandBuffers = &CommandBuffer;
    SubmitInfo.pWaitDstStageMask = &SubmitStageFlags;

    SubmitInfo.waitSemaphoreCount = (AcquireSemaphore_) ? 1 : 0;
    SubmitInfo.pWaitSemaphores = AcquireSemaphore_;
    SubmitInfo.signalSemaphoreCount = (ReleaseSemaphore_) ? 1 : 0;
    SubmitInfo.pSignalSemaphores = ReleaseSemaphore_;

    vkQueueSubmit(Queue, 1, &SubmitInfo, Fence);
    VK_CHECK(vkDeviceWaitIdle(LogicalDevice));
}

void vulkan_renderer::
BeginRendering()
{
    vkAcquireNextImageKHR(LogicalDevice, Swapchain, ~0ull, AcquireSemaphore, VK_NULL_HANDLE/*Here could be a fence*/, &ImageIndex);

    BeginCommands();

    VkClearColorValue Color = {0.086, 0.086, 0.113, 1};
    VkClearValue ClearColor = {Color};

    VkRenderPassBeginInfo RenderPassBeginInfo = {VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
    RenderPassBeginInfo.renderPass = RenderPass;
    RenderPassBeginInfo.framebuffer = SwapchainFramebuffers[ImageIndex];
    RenderPassBeginInfo.renderArea.extent.width = Width;
    RenderPassBeginInfo.renderArea.extent.height = Height;
    RenderPassBeginInfo.clearValueCount = 1;
    RenderPassBeginInfo.pClearValues = &ClearColor;

    vkCmdBeginRenderPass(CommandBuffer, &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport Viewport = {0, (r32)Height, (r32)Width, -(r32)Height};
    VkRect2D   Scissor  = {{0, 0}, {Width, Height}};

    vkCmdSetViewport(CommandBuffer, 0, 1, &Viewport);
    vkCmdSetScissor(CommandBuffer, 0, 1, &Scissor);
}

void vulkan_renderer::
DrawImage(image Image, v3 StartPointSrc, v3 StartPointDst)
{
    VkImageSubresourceLayers SubresourceSrc = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
    VkImageSubresourceLayers SubresourceDst = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
    VkOffset3D StartPosSrc = {0, 0, 0};
    VkOffset3D StartPosDst = {0, 0, 0};
    VkExtent3D Dimensions  = {Image.Width, Image.Height, 1};

    VkImageCopy ImageCopy = {SubresourceSrc, StartPosSrc, SubresourceDst, StartPosDst, Dimensions};

    vkCmdCopyImage(CommandBuffer, 
                   Image.Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 
                   SwapchainImages[ImageIndex], VK_IMAGE_LAYOUT_GENERAL,
                   1, &ImageCopy);
}

void vulkan_renderer::
EndRendering()
{
    vkCmdEndRenderPass(CommandBuffer);

    EndCommands(&AcquireSemaphore, &ReleaseSemaphore);

    VkPresentInfoKHR PresentInfo = {};
    PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    PresentInfo.waitSemaphoreCount = (u32)RenderingSemaphores.size();
    PresentInfo.pWaitSemaphores = (RenderingSemaphores.size() > 0) ? &RenderingSemaphores[0] : nullptr;
    PresentInfo.swapchainCount = 1;
    PresentInfo.pSwapchains = &Swapchain;
    PresentInfo.pImageIndices = &ImageIndex;
    PresentInfo.waitSemaphoreCount = 1;
    PresentInfo.pWaitSemaphores = &ReleaseSemaphore;

    VK_CHECK(vkQueuePresentKHR(Queue, &PresentInfo));

    VK_CHECK(vkDeviceWaitIdle(LogicalDevice));
}

vulkan_renderer::~vulkan_renderer()
{
    PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(Instance, "vkDestroyDebugReportCallbackEXT");

    vkDestroyFence(LogicalDevice, Fence, 0);
    vkDestroySemaphore(LogicalDevice, AcquireSemaphore, 0);
    vkDestroySemaphore(LogicalDevice, ReleaseSemaphore, 0);

    vkFreeCommandBuffers(LogicalDevice, CommandPool, 1, &CommandBuffer);
    vkDestroyCommandPool(LogicalDevice, CommandPool, nullptr);

    DestroySwapchain();

    vkDestroySurfaceKHR(Instance, Surface, nullptr);

    vkDestroyDebugReportCallbackEXT(Instance, DebugCallback, 0);
    vkDestroyInstance(Instance, nullptr);
}

#if 0
int main(int argc, char** argv)
{
    u32 WindowWidth  = 600;
    u32 WindowHeight = 600;

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* Window = SDL_CreateWindow("Vulkan Renderer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WindowWidth, WindowHeight, SDL_WINDOW_VULKAN|SDL_WINDOW_SHOWN);

    vulkan_renderer* VulkanRenderer = new vulkan_renderer(Window, WindowWidth, WindowHeight);
    VulkanRenderer->InitVulkanRenderer();

    buffer VertexBuffer = VulkanRenderer->AllocateBuffer(1024, BUFFER_TRANSFER_SRC, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    image TestImage = VulkanRenderer->CreateImage(32, 32, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    while(1)
    {
        VulkanRenderer->BeginRendering();
        VulkanRenderer->EndRendering();
    }

    return 0;
}
#endif
