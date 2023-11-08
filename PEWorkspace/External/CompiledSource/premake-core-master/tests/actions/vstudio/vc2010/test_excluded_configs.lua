--
-- tests/actions/vstudio/vc2010/test_excluded_configs.lua
-- Check handling of configurations which have been excluded from the build.
-- Copyright (c) 2012-2013 Jason Perkins and the Premake project
--

	local suite = test.declare("vs2010_excluded_configs")
	local vc2010 = premake.vstudio.vc2010


--
-- Setup/teardown
--

	local wks, prj

	function suite.setup()
		premake.action.set("vs2010")

		wks = workspace("MyWorkspace")
		configurations { "Debug", "Release" }
		platforms { "Zeus", "Ares" }
		language "C++"

		prj = project("MyProject")
		kind "ConsoleApp"
		links { "MyProject2", "MyProject3" }

		project("MyProject2")
		kind "StaticLib"

		project("MyProject3")
		kind "StaticLib"
		removeplatforms { "Ares" }
	end

	local function prepare(platform)
		local cfg = test.getconfig(prj, "Debug", platform)
		vc2010.linker(cfg)
	end



--
-- If a sibling is included in one configuration and excluded from
-- another, the included configuration should link as normal.
--

	function suite.normalLink_onIncludedConfig()
		prepare("Zeus")
		test.capture [[
<Link>
	<SubSystem>Console</SubSystem>
	<GenerateDebugInformation>false</GenerateDebugInformation>
	<EntryPointSymbol>mainCRTStartup</EntryPointSymbol>
</Link>
		]]
	end



--
-- If a sibling is included in one configuration and excluded from
-- another, the excluded configuration should force explicit linking
-- and not list the excluded library.
--

	function suite.explicitLink_onExcludedConfig()
		prepare("Ares")
		test.capture [[
<Link>
	<SubSystem>Console</SubSystem>
	<GenerateDebugInformation>false</GenerateDebugInformation>
	<AdditionalDependencies>bin\Ares\Debug\MyProject2.lib;%(AdditionalDependencies)</AdditionalDependencies>
	<EntryPointSymbol>mainCRTStartup</EntryPointSymbol>
</Link>
<ProjectReference>
	<LinkLibraryDependencies>false</LinkLibraryDependencies>
</ProjectReference>
		]]
	end
