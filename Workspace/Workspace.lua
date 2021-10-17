-- Copyright 2020 Dhiraj Wishal
-- SPDX-License-Identifier: Apache-2.0

---------- Workspace project description ----------

project "Workspace"
	kind "None"

	files {
		"%{wks.location}/Workspace/**.lua",
		"%{wks.location}/Workspace/**.vert",
		"%{wks.location}/Workspace/**.frag",
		"%{wks.location}/Workspace/**.comp",
		"%{wks.location}/Workspace/**.tesc",
		"%{wks.location}/Workspace/**.tese",
		"%{wks.location}/Workspace/**.geom",
	}