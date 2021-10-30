// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "MaterialPipelineUI.hpp"
#include "UIPresets.hpp"

namespace Flint
{
	namespace Helpers
	{
		const char* GetColorBlendFactorString(ColorBlendFactor factor)
		{
			switch (factor)
			{
			case Flint::ColorBlendFactor::Zero:								return "Zero";
			case Flint::ColorBlendFactor::One:								return "One";
			case Flint::ColorBlendFactor::SourceColor:						return "SourceColor";
			case Flint::ColorBlendFactor::OneMinusSourceColor:				return "OneMinusSourceColor";
			case Flint::ColorBlendFactor::DestinationColor:					return "DestinationColor";
			case Flint::ColorBlendFactor::OneMinusDestinationColor:			return "OneMinusDestinationColor";
			case Flint::ColorBlendFactor::SourceAlpha:						return "SourceAlpha";
			case Flint::ColorBlendFactor::OneMinusSourceAlpha:				return "OneMinusSourceAlpha";
			case Flint::ColorBlendFactor::DestinationAlpha:					return "DestinationAlpha";
			case Flint::ColorBlendFactor::OneMinusDestinationAlpha:			return "OneMinusDestinationAlpha";
			case Flint::ColorBlendFactor::ConstantColor:					return "ConstantColor";
			case Flint::ColorBlendFactor::OneMinusConstantColor:			return "OneMinusConstantColor";
			case Flint::ColorBlendFactor::ConstantAlpha:					return "ConstantAlpha";
			case Flint::ColorBlendFactor::OneMinusConstantAlpha:			return "OneMinusConstantAlpha";
			case Flint::ColorBlendFactor::SourceAlphaSaturate:				return "SourceAlphaSaturate";
			case Flint::ColorBlendFactor::SourceOneColor:					return "SourceOneColor";
			case Flint::ColorBlendFactor::OneMinusSourceOneColor:			return "OneMinusSourceOneColor";
			case Flint::ColorBlendFactor::SourceOneAlpha:					return "SourceOneAlpha";
			case Flint::ColorBlendFactor::OneMinusSourceOneAlpha:			return "OneMinusSourceOneAlpha";
			}

			return "";
		}

		const char* GetColorBlendOperatorString(ColorBlendOperator op)
		{
			switch (op)
			{
			case Flint::ColorBlendOperator::Add:							return "Add";
			case Flint::ColorBlendOperator::Subtract:						return "Subtract";
			case Flint::ColorBlendOperator::ReverseSubtract:				return "ReverseSubtract";
			case Flint::ColorBlendOperator::Minimum:						return "Minimum";
			case Flint::ColorBlendOperator::Maximum:						return "Maximum";
			case Flint::ColorBlendOperator::Zero:							return "Zero";
			case Flint::ColorBlendOperator::Source:							return "Source";
			case Flint::ColorBlendOperator::Destination:					return "Destination";
			case Flint::ColorBlendOperator::SourceOver:						return "SourceOver";
			case Flint::ColorBlendOperator::DestinationOver:				return "DestinationOver";
			case Flint::ColorBlendOperator::SourceIn:						return "SourceIn";
			case Flint::ColorBlendOperator::DestinationIn:					return "DestinationIn";
			case Flint::ColorBlendOperator::SouceOut:						return "SouceOut";
			case Flint::ColorBlendOperator::DestinationOut:					return "DestinationOut";
			case Flint::ColorBlendOperator::SourceATOP:						return "SourceATOP";
			case Flint::ColorBlendOperator::DestinationATOP:				return "DestinationATOP";
			case Flint::ColorBlendOperator::XOR:							return "XOR";
			case Flint::ColorBlendOperator::Multiply:						return "Multiply";
			case Flint::ColorBlendOperator::Screen:							return "Screen";
			case Flint::ColorBlendOperator::Overlay:						return "Overlay";
			case Flint::ColorBlendOperator::Darken:							return "Darken";
			case Flint::ColorBlendOperator::Lighten:						return "Lighten";
			case Flint::ColorBlendOperator::ColorDodge:						return "ColorDodge";
			case Flint::ColorBlendOperator::ColorBurn:						return "ColorBurn";
			case Flint::ColorBlendOperator::HardLight:						return "HardLight";
			case Flint::ColorBlendOperator::SoftLight:						return "SoftLight";
			case Flint::ColorBlendOperator::Difference:						return "Difference";
			case Flint::ColorBlendOperator::Exclusion:						return "Exclusion";
			case Flint::ColorBlendOperator::Invert:							return "Invert";
			case Flint::ColorBlendOperator::InvertRGB:						return "InvertRGB";
			case Flint::ColorBlendOperator::LinearDodge:					return "LinearDodge";
			case Flint::ColorBlendOperator::LinearBurn:						return "LinearBurn";
			case Flint::ColorBlendOperator::VividLight:						return "VividLight";
			case Flint::ColorBlendOperator::LinearLight:					return "LinearLight";
			case Flint::ColorBlendOperator::PinLight:						return "PinLight";
			case Flint::ColorBlendOperator::HardMix:						return "HardMix";
			case Flint::ColorBlendOperator::HSLHue:							return "HSLHue";
			case Flint::ColorBlendOperator::HSLSaturation:					return "HSLSaturation";
			case Flint::ColorBlendOperator::HSLColor:						return "HSLColor";
			case Flint::ColorBlendOperator::HSLLuminosity:					return "HSLLuminosity";
			case Flint::ColorBlendOperator::Plus:							return "Plus";
			case Flint::ColorBlendOperator::PlusClamped:					return "PlusClamped";
			case Flint::ColorBlendOperator::PlusClampedAlpha:				return "PlusClampedAlpha";
			case Flint::ColorBlendOperator::PlusDarker:						return "PlusDarker";
			case Flint::ColorBlendOperator::Minus:							return "Minus";
			case Flint::ColorBlendOperator::MinusClamped:					return "MinusClamped";
			case Flint::ColorBlendOperator::Contrast:						return "Contrast";
			case Flint::ColorBlendOperator::InvertOVG:						return "InvertOVG";
			case Flint::ColorBlendOperator::Red:							return "Red";
			case Flint::ColorBlendOperator::Green:							return "Green";
			case Flint::ColorBlendOperator::Blue:							return "Blue";
			}

			return "";
		}

		const char* GetPrimitiveTopologyString(PrimitiveTopology topology)
		{
			switch (topology)
			{
			case Flint::PrimitiveTopology::PointList:						return "PointList";
			case Flint::PrimitiveTopology::LineList:						return "LineList";
			case Flint::PrimitiveTopology::LineStrip:						return "LineStrip";
			case Flint::PrimitiveTopology::TriangleList:					return "TriangleList";
			case Flint::PrimitiveTopology::TriangleStrip:					return "TriangleStrip";
			case Flint::PrimitiveTopology::TriangleFan:						return "TriangleFan";
			case Flint::PrimitiveTopology::LineListWithAdjacency:			return "LineListWithAdjacency";
			case Flint::PrimitiveTopology::LineStripWithAdjacency:			return "LineStripWithAdjacency";
			case Flint::PrimitiveTopology::TriangleListWithAdjacency:		return "TriangleListWithAdjacency";
			case Flint::PrimitiveTopology::TriangleStripWithAdjacency:		return "TriangleStripWithAdjacency";
			case Flint::PrimitiveTopology::PatchList:						return "PatchList";
			}

			return "";
		}

		const char* GetCullModeString(CullMode mode)
		{
			switch (mode)
			{
			case Flint::CullMode::None:										return "None";
			case Flint::CullMode::Front:									return "Front";
			case Flint::CullMode::Back:										return "Back";
			case Flint::CullMode::FrontAndBack:								return "FrontAndBack";
			}

			return "";
		}

		const char* GetFrontFaceString(FrontFace face)
		{
			switch (face)
			{
			case Flint::FrontFace::CounterClockwise:						return "CounterClockwise";
			case Flint::FrontFace::Clockwise:								return "CounterClockwise";
			}

			return "";
		}

		const char* GetPolygonModeString(PolygonMode mode)
		{
			switch (mode)
			{
			case Flint::PolygonMode::Fill:									return "Fill";
			case Flint::PolygonMode::Line:									return "Line";
			case Flint::PolygonMode::Point:									return "Point";
			}

			return "";
		}

		const char* GetColorBlendLogicString(ColorBlendLogic logic)
		{
			switch (logic)
			{
			case Flint::ColorBlendLogic::Clear:								return "Clear";
			case Flint::ColorBlendLogic::And:								return "And";
			case Flint::ColorBlendLogic::AndReverse:						return "AndReverse";
			case Flint::ColorBlendLogic::Copy:								return "Copy";
			case Flint::ColorBlendLogic::AndInverted:						return "AndInverted";
			case Flint::ColorBlendLogic::NoOperator:						return "NoOperator";
			case Flint::ColorBlendLogic::XOR:								return "XOR";
			case Flint::ColorBlendLogic::OR:								return "OR";
			case Flint::ColorBlendLogic::NOR:								return "NOR";
			case Flint::ColorBlendLogic::Equivalent:						return "Equivalent";
			case Flint::ColorBlendLogic::Invert:							return "Invert";
			case Flint::ColorBlendLogic::ReverseOR:							return "ReverseOR";
			case Flint::ColorBlendLogic::CopyInverted:						return "CopyInverted";
			case Flint::ColorBlendLogic::InvertedOR:						return "InvertedOR";
			case Flint::ColorBlendLogic::NAND:								return "NAND";
			case Flint::ColorBlendLogic::Set:								return "Set";
			}

			return "";
		}

		const char* GetDepthCompareLogicString(DepthCompareLogic logic)
		{
			switch (logic)
			{
			case Flint::DepthCompareLogic::Never:							return "Never";
			case Flint::DepthCompareLogic::Less:							return "Less";
			case Flint::DepthCompareLogic::Equal:							return "Equal";
			case Flint::DepthCompareLogic::LessOrEqual:						return "LessOrEqual";
			case Flint::DepthCompareLogic::Greater:							return "Greater";
			case Flint::DepthCompareLogic::NotEqual:						return "NotEqual";
			case Flint::DepthCompareLogic::GreaterOrEqual:					return "GreaterOrEqual";
			case Flint::DepthCompareLogic::Always:							return "Always";
			}

			return "";
		}

		std::vector<std::string> GetDynamicStateFlagsStrings(DynamicStateFlags flags)
		{
			if (flags & DynamicStateFlags::Undefined)
				return { "Undefined" };

			std::vector<std::string> strings;
			if (flags & DynamicStateFlags::ViewPort)
				strings.push_back("ViewPort");

			if (flags & DynamicStateFlags::Scissor)
				strings.push_back("Scissor");

			if (flags & DynamicStateFlags::LineWidth)
				strings.push_back("LineWidth");

			if (flags & DynamicStateFlags::DepthBias)
				strings.push_back("DepthBias");

			if (flags & DynamicStateFlags::BlendConstants)
				strings.push_back("BlendConstants");

			if (flags & DynamicStateFlags::DepthBounds)
				strings.push_back("DepthBounds");

			return strings;
		}

		const char* GetMultiSampleCountString(MultiSampleCount count)
		{
			switch (count)
			{
			case Flint::MultiSampleCount::One:								return "One";
			case Flint::MultiSampleCount::Two:								return "Two";
			case Flint::MultiSampleCount::Four:								return "Four";
			case Flint::MultiSampleCount::Eight:							return "Eight";
			case Flint::MultiSampleCount::Sixteen:							return "Sixteen";
			case Flint::MultiSampleCount::ThirtyTwo:						return "ThirtyTwo";
			case Flint::MultiSampleCount::SixtyFour:						return "SixtyFour";
			}

			return "";
		}

		const char* GetResoureTypeString(ShaderResourceType type)
		{
			switch (type)
			{
			case Flint::ShaderResourceType::Sampler:						return "Sampler";
			case Flint::ShaderResourceType::CombinedImageSampler:			return "CombinedImageSampler";
			case Flint::ShaderResourceType::SampledImage:					return "SampledImage";
			case Flint::ShaderResourceType::StorageImage:					return "StorageImage";
			case Flint::ShaderResourceType::UniformTexelBuffer:				return "UniformTexelBuffer";
			case Flint::ShaderResourceType::StorageTexelBuffer:				return "StorageTexelBuffer";
			case Flint::ShaderResourceType::UniformBuffer:					return "UniformBuffer";
			case Flint::ShaderResourceType::StorageBuffer:					return "StorageBuffer";
			case Flint::ShaderResourceType::UniformBufferDynamic:			return "UniformBufferDynamic";
			case Flint::ShaderResourceType::StorageBufferDynamic:			return "StorageBufferDynamic";
			case Flint::ShaderResourceType::InputAttachment:				return "InputAttachment";
			case Flint::ShaderResourceType::AccelerationStructure:			return "AccelerationStructure";
			}

			return "";
		}
	}

	MaterialPipelineUI::MaterialPipelineUI(ComponentIdentifier& identifier, const Components::MaterialPipeline& pipeline)
	{
		ImGui::Begin(std::string("Material Pipeline - " + std::to_string(identifier.mStringHash)).c_str());

		const auto spec = pipeline.pPipeline->GetSpecification();

		ImGui::TextColored(TitleColor, "Input Assembly");
		ImGui::RadioButton("Enable primitive restart", spec.bEnablePrimitiveRestart);
		ImGui::Text("Primitive topology: %s", Helpers::GetPrimitiveTopologyString(spec.mPrimitiveTopology));
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::TextColored(TitleColor, "Tessellation Configuration");
		ImGui::Text("Tessellation patch control points: %u", spec.mTessellationPatchControlPoints);
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::TextColored(TitleColor, "Color Blending");
		for (uint64 i = 0; i < spec.mColorBlendAttachments.size(); i++)
		{
			const ColorBlendAttachment attachment = spec.mColorBlendAttachments[i];

			ImGui::Text("Color Blend Attachment %u", i);
			ImGui::Indent();

			ImGui::RadioButton("Enabled", attachment.mEnableBlend);
			ImGui::Text("Source blend factor: %s", Helpers::GetColorBlendFactorString(attachment.mSrcBlendFactor));
			ImGui::Text("Destination blend factor: %s", Helpers::GetColorBlendFactorString(attachment.mDstBlendFactor));
			ImGui::Text("Source alpha blend factor: %s", Helpers::GetColorBlendFactorString(attachment.mSrcAlphaBlendFactor));
			ImGui::Text("Destination alpha blend factor: %s", Helpers::GetColorBlendFactorString(attachment.mDstAlphaBlendFactor));
			ImGui::Text("Blend operator: %s", Helpers::GetColorBlendOperatorString(attachment.mBlendOperator));
			ImGui::Text("Alpha blend operator: %s", Helpers::GetColorBlendOperatorString(attachment.mAlphaBlendOperator));

			std::string writeMask = "";
			if ((attachment.mColorWriteMask & ColorWriteMask::R) == ColorWriteMask::R)
				writeMask.push_back('R');

			if ((attachment.mColorWriteMask & ColorWriteMask::G) == ColorWriteMask::G)
				writeMask.push_back('G');

			if ((attachment.mColorWriteMask & ColorWriteMask::B) == ColorWriteMask::B)
				writeMask.push_back('B');

			if ((attachment.mColorWriteMask & ColorWriteMask::A) == ColorWriteMask::A)
				writeMask.push_back('A');

			ImGui::Text("Color write mask: %s", writeMask.c_str());

			ImGui::Unindent();
		}

		ImGui::RadioButton("Enable color blend logic", spec.bEnableColorBlendLogic);
		ImGui::Text("Color blend logic: %s", Helpers::GetColorBlendLogicString(spec.mColorBlendLogic));

		float color4[4] = { spec.mColorBlendConstants[0], spec.mColorBlendConstants[1], spec.mColorBlendConstants[2], spec.mColorBlendConstants[3] };
		ImGui::Text("Color blend constants:");
		ImGui::Indent();
		ImGui::ColorEdit4("", color4);
		ImGui::Unindent();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::TextColored(TitleColor, "Rasterizer");
		ImGui::Text("Cull mode: %s", Helpers::GetCullModeString(spec.mCullMode));
		ImGui::RadioButton("Enable depth bias", spec.bEnableDepthBias);
		ImGui::Text("Depth bias factor: %f", spec.mDepthBiasFactor);
		ImGui::Text("Depth constant factor: %f", spec.mDepthConstantFactor);
		ImGui::Text("Depth slope factor: %f", spec.mDepthSlopeFactor);
		ImGui::RadioButton("Enable depth clamp", spec.bEnableDepthClamp);
		ImGui::Text("Front face: %s", Helpers::GetFrontFaceString(spec.mFrontFace));
		ImGui::Text("Rasterizer line width: %f", spec.mRasterizerLineWidth);
		ImGui::Text("Polygon mode: %s", Helpers::GetPolygonModeString(spec.mPolygonMode));
		ImGui::RadioButton("Enable rasterizer discard", spec.bEnableRasterizerDiscard);
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::TextColored(TitleColor, "Multi Sampling");
		ImGui::RadioButton("Enable alpha coverage", spec.bEnableAlphaCoverage);
		ImGui::RadioButton("Enable alpha to one", spec.bEnableAlphaToOne);
		ImGui::Text("Minimum sample shading: %f", spec.mMinSampleShading);
		ImGui::Text("Rasterization samples: %s", Helpers::GetMultiSampleCountString(spec.mRasterizationSamples));
		ImGui::RadioButton("Enable sample shading", spec.bEnableSampleShading);
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::TextColored(TitleColor, "Depth Testing");
		ImGui::RadioButton("Enable depth test", spec.bEnableDepthTest);
		ImGui::RadioButton("Enable depth write", spec.bEnableDepthWrite);
		ImGui::Text("Depth compare logic: %s", Helpers::GetDepthCompareLogicString(spec.mDepthCompareLogic));
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::TextColored(TitleColor, "Dynamic States");
		ImGui::Text("Dynamic state flags:");
		ImGui::Indent();
		for (const auto string : Helpers::GetDynamicStateFlagsStrings(spec.mDynamicStateFlags))
			ImGui::Text(string.c_str());
		ImGui::Unindent();
		ImGui::Separator();
		ImGui::Spacing();

		if (pipeline.pVertexShader)
			DisplayShader(pipeline.pVertexShader, "Vertex Shader");

		if (pipeline.pTessellationControlShader)
			DisplayShader(pipeline.pTessellationControlShader, "Tessellation Control Shader");

		if (pipeline.pTessellationEvaluationShader)
			DisplayShader(pipeline.pTessellationEvaluationShader, "Tessellation Evaluation Shader");

		if (pipeline.pGeometryShader)
			DisplayShader(pipeline.pGeometryShader, "Geometry Shader");

		if (pipeline.pFragmentShader)
			DisplayShader(pipeline.pFragmentShader, "Fragment Shader");
	}

	MaterialPipelineUI::~MaterialPipelineUI()
	{
		ImGui::End();
	}
	
	void MaterialPipelineUI::DisplayShader(const std::shared_ptr<Shader>& pShader, const std::string& title)
	{
		ImGui::TextColored(TitleColor, title.c_str());

		ImGui::Text("Input attributes:");
		ImGui::Indent();
		for (const auto binding : pShader->GetInputAttributes())
		{
			ImGui::Text("Binding: %u", binding.first);

			ImGui::Indent();
			for (const auto attribute : binding.second)
			{
				ImGui::Text("Location: %u", attribute.mLocation);
				ImGui::Text("Variable name: %s", attribute.mAttributeName.c_str());
				ImGui::Text("Size: %u bytes", static_cast<uint8>(attribute.mDataType));
				ImGui::Spacing();
			}
			ImGui::Unindent();
		}

		ImGui::Unindent();
		ImGui::Text("Output attributes:");
		ImGui::Indent();
		for (const auto binding : pShader->GetOutputAttributes())
		{
			ImGui::Text("Binding: %u", binding.first);

			ImGui::Indent();
			for (const auto attribute : binding.second)
			{
				ImGui::Text("Location: %u", attribute.mLocation);
				ImGui::Text("Variable name: %s", attribute.mAttributeName.c_str());
				ImGui::Text("Size: %u bytes", static_cast<uint8>(attribute.mDataType));
				ImGui::Spacing();
			}
			ImGui::Unindent();
		}

		ImGui::Unindent();
		ImGui::Text("Resources:");
		ImGui::Indent();
		for (const auto binding : pShader->GetShaderResources())
		{
			ImGui::Text("Set: %u", binding.first);

			ImGui::Indent();
			for (const auto resource : binding.second)
			{
				ImGui::Text("Binding: %u", resource.first);
				ImGui::Text("Type: %s", Helpers::GetResoureTypeString(resource.second));
				ImGui::Spacing();
			}
			ImGui::Unindent();
		}

		ImGui::Unindent();
		ImGui::Separator();
		ImGui::Spacing();
	}
}