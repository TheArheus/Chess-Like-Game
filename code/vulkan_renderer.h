
#include <initializer_list>
#define VK_USE_PLATFORM_WIN32_KHR
#include <SDL2/SDL_config.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL.h>
#undef main

#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan.h>

#include <iostream>

#include "intrinsics.h"
#include "hmath.h"

#define VK_CHECK(Error) \
{ \
    do \
    { \
        VkResult FunctionResult = Error; \
        if(FunctionResult) \
        { \
            std::cout << "Vulkan Function Error: " << FunctionResult << std::endl; \
            abort(); \
        } \
    }while(0); \
}

#define BUFFER_TRANSFER_SRC  VK_BUFFER_USAGE_TRANSFER_SRC_BIT
#define BUFFER_TRANSFER_DST  VK_BUFFER_USAGE_TRANSFER_DST_BIT
#define BUFFER_UNIFORM_TEXEL VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
#define BUFFER_STORAGE_TEXEL VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
#define BUFFER_UNIFORM       VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
#define BUFFER_INDEX         VK_BUFFER_USAGE_INDEX_BUFFER_BIT
#define BUFFER_VERTEX        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
#define BUFFER_INDIRECT      VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT

struct buffer
{
    VkBuffer Buffer;
    VkDeviceMemory Memory;
    void* Data;
    size_t Size;
};

struct image
{
    VkImage Image;
    VkImageView View;
    VkDeviceMemory Memory;
    void* Data;
    u32 Width;
    u32 Height;
};

struct shader
{
    VkShaderModule Module;
    VkShaderStageFlagBits Stage;
};

struct material
{
    VkPipeline Pipeline;
    VkPipelineLayout PipelineLayout;
};

using shaders = std::initializer_list<const shader*>;

class vulkan_renderer 
{
private:
    u32 Width;
    u32 Height;

    u32 ImageIndex;

    u32 QueueFamilyIndex;

    SDL_Window* Window;

    VkInstance Instance;
    VkPhysicalDevice PhysicalDevice;
    VkPhysicalDeviceMemoryProperties MemProperty;
    VkDevice LogicalDevice;
    VkQueue Queue;

    VkFence Fence;
    VkSemaphore AcquireSemaphore;
    VkSemaphore ReleaseSemaphore;
    std::vector<VkSemaphore> RenderingSemaphores;

    VkCommandPool CommandPool;
    VkCommandBuffer CommandBuffer;

    VkSurfaceKHR Surface;
    VkSwapchainKHR Swapchain;
    VkSurfaceFormatKHR SwapchainSurfaceFormat;

    VkDebugReportCallbackEXT DebugCallback;

    std::vector<shader> ShaderModules;
    std::vector<VkImage> SwapchainImages;
    std::vector<VkImageView> SwapchainImageViews;
    std::vector<VkFramebuffer> SwapchainFramebuffers;

    std::unordered_map<std::string, material> Materials;

    VkRenderPass RenderPass;
    VkSampler MainImageSampler;

    VkPipeline MainPipeline;
    VkPipelineLayout MainPipelineLayout;
    VkDescriptorSet MainDescriptor;

    VkBufferMemoryBarrier CreateMemoryBarrier(buffer& Buffer, VkAccessFlags CurrentAccess, VkAccessFlags NewAccess);
    VkImageMemoryBarrier CreateImageBarrier(image& Image, VkAccessFlags OldAccess, VkAccessFlags NewAccess, VkImageLayout OldLayout, VkImageLayout NewLayout);
    VkSampler CreateSampler(VkFilter Filter = VK_FILTER_LINEAR, VkSamplerAddressMode AddressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT);
    VkDescriptorSetLayout CreateDescriptorSetLayout();
    VkDescriptorPool CreateDescriptorPool();

    VkImageView CreateImageView(VkImage Image);
    VkFramebuffer CreateFramebuffer(VkImageView ImageView_);
    void UpdateImageLayout(image& Image, VkImageLayout OldLayout, VkImageLayout NewLayout);

    void CreateRenderPass();

    VkSemaphore CreateSemaphore();
    VkFence CreateFence();

public:
    vulkan_renderer(SDL_Window* Window_, u32 Width_, u32 Height_);
    ~vulkan_renderer();

    buffer AllocateBuffer(size_t Size, VkBufferUsageFlags BufferUsage, VkMemoryPropertyFlags MemoryFlags);

    void InitVulkanRenderer();
    void InitGraphicsPipeline();
    void CreateSwapchain(u32 WindowWidth_ = 0, u32 WindowHeight_ = 0);
    void DestroySwapchain();

    void BeginCommands();
    void EndCommands(VkSemaphore* AcquireSemaphore_ = nullptr, VkSemaphore* ReleaseSemaphore_ = nullptr);

    VkCommandBuffer BeginCommand();
    void EndCommand(VkCommandBuffer CommandBufferResult);

    void BeginRendering();
    void EndRendering();

    void BindBuffer(buffer& VertexBuffer, u32 Binding);
    void BindImage(image& Image, u32 Binding);

    void UpdateBuffer(buffer& Buffer, buffer& Scratch, void* Data, size_t Size, size_t Offset = 0);
    void UpdateBuffer(buffer& Buffer, buffer& Scratch, size_t Size, size_t Offset = 0);
    void UpdateTexture(image& Image, buffer& Scratch, size_t Offset = 0);

    void DrawImage(image Image, v3 StartPointSrc = V3(0, 0, 0), v3 StartPointDst = V3(0, 0, 0));
    void DrawMeshes(buffer& VertexBuffer, buffer& IndexBuffer, image& Image);

    VkWriteDescriptorSet WriteBuffer(VkDescriptorBufferInfo* BufferInfo, VkDescriptorSet Set, VkDescriptorType DescriptorType, u32 Binding);
    VkWriteDescriptorSet WriteImage(VkDescriptorImageInfo* ImageInfo, VkDescriptorSet Set, VkDescriptorType DescriptorType, u32 Binding);

    void UploadShader(const char* Path, VkShaderStageFlagBits Stages);

    image CreateImage(u32 ImageWidth, u32 ImageHeight, VkImageUsageFlags Usage, VkMemoryPropertyFlags MemoryFlags, u32 LayersCount = 1, VkBool32 ShouldBeCubemap = 0);
};
