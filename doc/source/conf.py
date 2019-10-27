# -*- coding: utf-8 -*-
#
# Configuration file for the Sphinx documentation builder.
#
# This file does only contain a selection of the most common options. For a
# full list see the documentation:
# http://www.sphinx-doc.org/en/master/config

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
import subprocess
import os
import textwrap
# import sys
# sys.path.insert(0, os.path.abspath('.'))

# This is an homemade environment variable set by the user to reduce the
# compilation time if 'SPHINX_BUILDERNAME' == 'latex'. It that case, the
# 'breathe' and 'exhale' extensions are disabled since the final LaTeX PDF do
# not include the API documentation.
buildername = str(os.getenv('SPHINX_BUILDERNAME'))

# -- Project information -----------------------------------------------------

project = 'AFF3CT'
copyright = '2019, AFF3CT team'
author = 'AFF3CT team'

# get the AFF3CT version from Git
label = subprocess.check_output(["git", "describe"]).strip().decode(encoding='UTF-8')
split_label = label.split("-")

# The short X.Y version
version = split_label[0]
# The full version, including alpha/beta/rc tags
release = label

# -- General configuration ---------------------------------------------------

# If your documentation needs a minimal Sphinx version, state it here.
#
# needs_sphinx = '1.0'

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    'sphinx.ext.autodoc',
    'sphinx.ext.mathjax',
    'sphinxcontrib.bibtex',
    'sphinxcontrib.rsvgconverter',
    # 'm2r',
]

if buildername != "latex":
    extensions.append('breathe')
    extensions.append('exhale')

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# The suffix(es) of source filenames.
# You can specify multiple suffix as a list of string:
#
# source_suffix = ['.rst', '.md']
source_suffix = '.rst'

# The master toctree document.
master_doc = 'index'

# The language for content autogenerated by Sphinx. Refer to documentation
# for a list of supported languages.
#
# This is also used if you do content translation via gettext catalogs.
# Usually you set "language" from the command line for these cases.
language = None

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = []

# Figures, tables and code-blocks are automatically numbered if they have a caption
numfig = True

# The name of the Pygments (syntax highlighting) style to use.
pygments_style = None

# -- Options for HTML output -------------------------------------------------

# 'read_the_docs_build' is whether we are on readthedocs.org, this line of code
# grabbed from docs.readthedocs.org
read_the_docs_build = os.environ.get('READTHEDOCS', None) == 'True'

if not read_the_docs_build:  # only import and set the theme if we're building docs locally
    import sphinx_rtd_theme
    # The theme to use for HTML and HTML Help pages.  See the documentation for
    # a list of builtin themes.
    html_theme = 'sphinx_rtd_theme'
    html_theme_path = [sphinx_rtd_theme.get_html_theme_path()]


# Theme options are theme-specific and customize the look and feel of a theme
# further.  For a list of options available for each theme, see the
# documentation.
# see here for description : https://sphinx-rtd-theme.readthedocs.io/en/latest/configuring.html#html-theme-options
# TODO : Why the compilation fails the first time ???? -> the second time is good.
html_theme_options = {
    'canonical_url': '', # to help search engines with duplicated versions of the doc -> TODO
    'style_external_links': False, # Add an icon next to external links.
    'display_version': True, # the version number shown at the top of the sidebar
    # Toc options
    'navigation_depth' : -1,
    'collapse_navigation': True,
    'sticky_navigation': True,
    'includehidden': False,
    'titles_only': False
}

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

# Custom sidebar templates, must be a dictionary that maps document names
# to template names.
#
# The default sidebars (for documents that don't match any pattern) are
# defined by theme itself.  Builtin themes are using these templates by
# default: ``['localtoc.html', 'relations.html', 'sourcelink.html',
# 'searchbox.html']``.
#
# html_sidebars = {}
html_favicon = None
html_logo    = None

# -- Options for HTMLHelp output ---------------------------------------------

# Output file base name for HTML help builder.
htmlhelp_basename = 'AFF3CTdoc'

# -- Options for LaTeX output ------------------------------------------------

latex_elements = {
    # The paper size ('letterpaper' or 'a4paper').
    #
    # 'papersize': 'letterpaper',

    # The font size ('10pt', '11pt' or '12pt').
    #
    # 'pointsize': '10pt',

    # Additional stuff for the LaTeX preamble.
    #
    # 'preamble': '',

    # Latex figure (float) alignment
    #
    # 'figure_align': 'htbp',

    'preamble': '\setcounter{tocdepth}{10}'
}

# Grouping the document tree into LaTeX files. List of tuples
# (source start file, target name, title,
#  author, documentclass [howto, manual, or own class]).
latex_documents = [
    (master_doc, 'AFF3CT.tex', 'AFF3CT Documentation',
     'AFF3CT team', 'manual'),
]

# -- Options for manual page output ------------------------------------------

# One entry per manual page. List of tuples
# (source start file, name, description, authors, manual section).
man_pages = [
    (master_doc, 'aff3ct', 'AFF3CT Documentation',
     [author], 1)
]

# -- Options for Texinfo output ----------------------------------------------

# Grouping the document tree into Texinfo files. List of tuples
# (source start file, target name, title, author,
#  dir menu entry, description, category)
texinfo_documents = [
    (master_doc, 'AFF3CT', 'AFF3CT Documentation',
     author, 'AFF3CT', 'One line description of project.',
     'Miscellaneous'),
]

# -- Options for Epub output -------------------------------------------------

# Bibliographic Dublin Core info.
epub_title = project

# The unique identifier of the text. This can be a ISBN number
# or the project homepage.
#
# epub_identifier = ''

# A unique identification for the text.
#
# epub_uid = ''

# A list of files that should not be packed into the epub file.
epub_exclude_files = ['search.html']

rst_epilog = """

.. |logo_ims| image:: https://www.ims-bordeaux.fr/images/logoimsjoom.png
    :alt: logo-ims-bordeaux
    :width: 60px
    :height: 30px

.. |image_required_argument| image:: /user/simulation/parameters/images/required.svg
                             :alt:  Required
                             :width:  80px
                             :height: 25px

.. |image_advanced_argument| image:: /user/simulation/parameters/images/advanced.svg
                             :alt:  Advanced
                             :width:  80px
                             :height: 25px

.. |AFF3CT|    replace:: :abbr:`AFF3CT   (A Fast Forward Error Correction Toolbox!)`
.. |AMS|       replace:: :abbr:`AMS      (Approximate Min-Star)`
.. |API|       replace:: :abbr:`API      (Application Programming Interface)`
.. |ARM|       replace:: :abbr:`ARM      (Advanced RISC (Reduced Instruction Set Computer) Machine)`
.. |ARMv7|     replace:: :abbr:`ARMv7    (Advanced RISC (Reduced Instruction Set Computer) Machine Vesion 7)`
.. |ARMv8|     replace:: :abbr:`ARMv8    (Advanced RISC (Reduced Instruction Set Computer) Machine Vesion 8)`
.. |ARP|       replace:: :abbr:`ARP      (Almost Regular Permutation)`
.. |ASCII|     replace:: :abbr:`ASCII    (American Standard Code for Information Interchange)`
.. |A-SCL|     replace:: :abbr:`A-SCL    (Adaptive Successive Cancellation List)`
.. |FA-SCL|    replace:: :abbr:`FA-SCL   (Fully Adaptive Successive Cancellation List)`
.. |PA-SCL|    replace:: :abbr:`PA-SCL   (Partially Adaptive Successive Cancellation List)`
.. |AVX|       replace:: :abbr:`AVX      (Advanced Vector Extensions)`
.. |AVX2|      replace:: :abbr:`AVX2     (Advanced Vector Extensions 2)`
.. |AVX-512|   replace:: :abbr:`AVX-512  (Advanced Vector Extensions 512-bit)`
.. |AVX-512F|  replace:: :abbr:`AVX-512F (Advanced Vector Extensions 512-bit Foundation)`
.. |AVX-512BW| replace:: :abbr:`AVX-512BW(Advanced Vector Extensions 512-bit Bytes-Words)`
.. |AWGN|      replace:: :abbr:`AWGN     (Additive White Gaussian Noise)`
.. |AZCW|      replace:: :abbr:`AZCW     (All Zero Code Word)`
.. |AZCWs|     replace:: :abbr:`AZCWs    (All Zero Code Words)`
.. |BCH|       replace:: :abbr:`BCH      (Bose, Ray-Chaudhuri and Hocquenghem)`
.. |BCJR|      replace:: :abbr:`BCJR     (Bahl, Cocke, Jelinek and Raviv algorithm or Maximum A Posteriori (MAP))`
.. |BEC|       replace:: :abbr:`BEC      (Binary Erasure Channel)`
.. |BER|       replace:: :abbr:`BER      (Bit Error Rate)`
.. |BF|        replace:: :abbr:`BF       (Bit Flipping)`
.. |BFER|      replace:: :abbr:`BER/FER  (Bit and Frame Error Rate)`
.. |BPSK|      replace:: :abbr:`BPSK     (Bit Phase-Shift Keying)`
.. |BM|        replace:: :abbr:`BM       (Berlekamp-Massey)`
.. |BP|        replace:: :abbr:`BP       (Belief Propagation)`
.. |BP-F|      replace:: :abbr:`BP-F     (Belief Propagation with Flooding scheduling)`
.. |BP-HL|     replace:: :abbr:`BP-HL    (Belief Propagation with Horizontal Layered scheduling)`
.. |BP-P|      replace:: :abbr:`BP-P     (Belief Propagation Peeling)`
.. |BP-VL|     replace:: :abbr:`BP-VL    (Belief Propagation with Vertical Layered scheduling)`
.. |BPS|       replace:: :abbr:`BPS      (Bit Per Symbol)`
.. |BSC|       replace:: :abbr:`BSC      (Binary Symmetric Channel)`
.. |CA|        replace:: :abbr:`CA       (CRC Aided)`
.. |CCSDS|     replace:: :abbr:`CCSDS    (Consultative Committee for Space Data Systems)`
.. |CDF|       replace:: :abbr:`CDF      (Cumulative Distribution Function)`
.. |CISC|      replace:: :abbr:`CISC     (Complex Instruction Set Computer)`
.. |CN|        replace:: :abbr:`CN       (Check Node)`
.. |CNs|       replace:: :abbr:`CNs      (Check Nodes)`
.. |codec|     replace:: :abbr:`codec    (coder/decoder)`
.. |codecs|    replace:: :abbr:`codecs   (coders/decodes)`
.. |CP|        replace:: :abbr:`CP       (Chase-Pyndiah)`
.. |CPM|       replace:: :abbr:`CPM      (Continuous Phase Modulation)`
.. |CPU|       replace:: :abbr:`CPU      (Central Process Unit)`
.. |CPUs|      replace:: :abbr:`CPUs     (Central Process Units)`
.. |CRC|       replace:: :abbr:`CRC      (Cyclic Redundancy Check)`
.. |CRCs|      replace:: :abbr:`CRCs     (Cyclic Redundancy Checks)`
.. |CSV|       replace:: :abbr:`CSV      (Comma-Separated Values)`
.. |DB|        replace:: :abbr:`DB       (Double Binary)`
.. |DE|        replace:: :abbr:`DE       (Density Evolution)`
.. |DVB-RCS1|  replace:: :abbr:`DVB-RCS1 (Digital Video Broadcasting - Return Channel via Satellite 1)`
.. |DVB-RCS2|  replace:: :abbr:`DVB-RCS2 (Digital Video Broadcasting - Return Channel via Satellite 2)`
.. |DVB-S1|    replace:: :abbr:`DVB-S1   (Digital Video Broadcasting - Satellite 1)`
.. |DVB-S2|    replace:: :abbr:`DVB-S2   (Digital Video Broadcasting - Satellite 2)`
.. |EP|        replace:: :abbr:`EP       (Event Probability)`
.. |EXIT|      replace:: :abbr:`EXIT     (EXtrinsic Information Transfer chart)`
.. |FER|       replace:: :abbr:`FER      (Frame Error Rate)`
.. |FNC|       replace:: :abbr:`FNC      (Flip aNd Check)`
.. |GA|        replace:: :abbr:`GA       (Gaussian Approximation)`
.. |GALA|      replace:: :abbr:`GALA     (Gallager A)`
.. |GALB|      replace:: :abbr:`GALB     (Gallager B)`
.. |GALE|      replace:: :abbr:`GALE     (Gallager E)`
.. |GNU|       replace:: :abbr:`GNU      (GNU's Not Unix!)`
.. |GPP|       replace:: :abbr:`GPP      (General Purpose Processor)`
.. |GPPs|      replace:: :abbr:`GPPs     (General Purpose Processors)`
.. |GPU|       replace:: :abbr:`GPU      (Graphics Processing Unit)`
.. |GPUs|      replace:: :abbr:`GPUs     (Graphics Processing Units)`
.. |GSL|       replace:: :abbr:`GSL      (GNU Scientific Library)`
.. |GSM|       replace:: :abbr:`GSM      (Global System for Mobile Communications)`
.. |GUI|       replace:: :abbr:`GUI      (Graphical User Interface)`
.. |icpc|      replace:: :abbr:`icpc     (Intel C++ Compiler)`
.. |IEEE|      replace:: :abbr:`IEEE     (Institute of Electrical and Electronics Engineers)`
.. |IFL|       replace:: :abbr:`IFL      (Inter Frame Level)`
.. |IRA|       replace:: :abbr:`IRA      (Irregular Repeat Accumulate)`
.. |ISA|       replace:: :abbr:`ISA      (Instruction Set Architecture)`
.. |ISAs|      replace:: :abbr:`ISAs     (Instruction Set Architectures)`
.. |JSON|      replace:: :abbr:`JSON     (JavaScript Object Notation)`
.. |LDPC|      replace:: :abbr:`LDPC     (Low-Density Parity-Check)`
.. |LLRs|      replace:: :abbr:`LLRs     (Log Likelihood Ratios)`
.. |LLR|       replace:: :abbr:`LLR      (Log Likelihood Ratio)`
.. |LSPA|      replace:: :abbr:`LSPA     (Logarithmic Sum-Product Algorithm)`
.. |LTE|       replace:: :abbr:`LTE      (Long Term Evolution)`
.. |LUT|       replace:: :abbr:`LUT      (Look Up Table)`
.. |LUTs|      replace:: :abbr:`LUTs     (Look Up Tables)`
.. |MAP|       replace:: :abbr:`MAP      (Maximum A Posteriori)`
.. |MATLAB|    replace:: MATLAB
.. |MI|        replace:: :abbr:`MI       (Mutual Information)`
.. |MKL|       replace:: :abbr:`MKL      (Intel Math Kernel Library)`
.. |ML|        replace:: :abbr:`ML       (Maximum Likelihood)`
.. |modem|     replace:: :abbr:`modem    (modulator/demodulator)`
.. |modems|    replace:: :abbr:`modems   (modulators/demodulators)`
.. |MPI|       replace:: :abbr:`MPI      (Message Passing Interface)`
.. |MS|        replace:: :abbr:`MS       (Min-Sum)`
.. |MSVC|      replace:: :abbr:`MSVC     (Microsoft Visual C++)`
.. |MT 19937|  replace:: :abbr:`MT 19937 (Mersenne Twister 19937)`
.. |MWBF|      replace:: :abbr:`MWBF     (Modified Weighted Bit Flipping)`
.. |NEON|      replace:: :abbr:`NEON     (ARM SIMD instructions)`
.. |NMS|       replace:: :abbr:`NMS      (Normalized Min-Sum)`
.. |OMS|       replace:: :abbr:`OMS      (Offset Min-Sum)`
.. |ONMS|      replace:: :abbr:`ONMS     (Offset Normalized Min-Sum)`
.. |OOK|       replace:: :abbr:`OOK      (On-Off Keying)`
.. |OS|        replace:: :abbr:`OS       (Operating System)`
.. |OSs|       replace:: :abbr:`OSs      (Operating Systems)`
.. |PAM|       replace:: :abbr:`PAM      (Pulse-Amplitude Modulation)`
.. |PDF|       replace:: :abbr:`PDF      (Probability Density Function)`
.. |PRNG|      replace:: :abbr:`PRNG     (Pseudo Random Number Generator)`
.. |PRNGs|     replace:: :abbr:`PRNGs    (Pseudo Random Number Generators)`
.. |PSK|       replace:: :abbr:`PSK      (Phase-Shift Keying)`
.. |PPA|       replace:: :abbr:`PPA      (Personal Package Archive)`
.. |PPBF|      replace:: :abbr:`PPBF     (Probabilistic Parallel Bit-Flipping)`
.. |PyBER|     replace:: PyBER
.. |QAM|       replace:: :abbr:`QAM      (Quadrature Amplitude Modulation)`
.. |QC|        replace:: :abbr:`QC       (Quasi-Cyclic)`
.. |RA|        replace:: :abbr:`RA       (Repeat and Accumulate)`
.. |release|   replace:: """ + release + """
.. |RISC|      replace:: :abbr:`RISC     (Reduced Instruction Set Computer)`
.. |ROP|       replace:: :abbr:`ROP      (Received Optical Power)`
.. |RSC|       replace:: :abbr:`RSC      (Recursive Systematic Convolutional)`
.. |RS|        replace:: :abbr:`RS       (Reed-Solomon)`
.. |SC|        replace:: :abbr:`SC       (Successive Cancellation)`
.. |SCAN|      replace:: :abbr:`SCAN     (Soft CANcellation)`
.. |SCF|       replace:: :abbr:`SCF      (Successive Cancellation Flip)`
.. |SCL|       replace:: :abbr:`SCL      (Successive Cancellation List)`
.. |SCMA|      replace:: :abbr:`SCMA     (Sparse Code Multiple Access)`
.. |SCo|       replace:: :abbr:`SC       (Self-Corrected)`
.. |SDR|       replace:: :abbr:`SDR      (Software-Defined Radio)`
.. |SF|        replace:: :abbr:`SF       (Scaling Factor)`
.. |SFs|       replace:: :abbr:`SFs      (Scaling Factors)`
.. |SIMD|      replace:: :abbr:`SIMD     (Single Instruction Multiple Data)`
.. |SNRs|      replace:: :abbr:`SNRs     (Signal Noise Ratios)`
.. |SNR|       replace:: :abbr:`SNR      (Signal Noise Ratio)`
.. |SPC|       replace:: :abbr:`SPC      (Single Parity Check)`
.. |SPA|       replace:: :abbr:`SPA      (Sum-Product Algorithm)`
.. |SSE|       replace:: :abbr:`SSE      (Streaming SIMD Extensions)`
.. |SSE2|      replace:: :abbr:`SSE2     (Streaming SIMD Extensions 2)`
.. |SSE3|      replace:: :abbr:`SSE3     (Streaming SIMD Extensions 3)`
.. |SSSE3|     replace:: :abbr:`SSSE3    (Supplemental Streaming SIMD Extensions 3)`
.. |SSE4.1|    replace:: :abbr:`SSE4.1   (Streaming SIMD Extensions 4.1)`
.. |SSE4.2|    replace:: :abbr:`SSE4.2   (Streaming SIMD Extensions 4.2)`
.. |STD|       replace:: :abbr:`STD      (Standard)`
.. |TPC|       replace:: :abbr:`TPC      (Turbo Product Code)`
.. |TV|        replace:: :abbr:`TV       (Tal & Vardy)`
.. |version|   replace:: """ + version + """
.. |VN|        replace:: :abbr:`VN       (Variable Node)`
.. |VNs|       replace:: :abbr:`VNs      (Variable Nodes)`
.. |WBF|       replace:: :abbr:`WBF      (Weighted Bit Flipping)`

"""

# parse the strings.rst file and add its contents to the 'rst_epilog' variable
with open("../strings.rst", "r") as ins:
    for line in ins:
        rst_epilog = rst_epilog + line;

# -- Extension configuration -------------------------------------------------

# -- Configure Breathe (Developer doc from Doxygen XML files)

if buildername != "latex":

    # Uncomment the following lines to enable the Doxygen compilation
    # If we are on a Readthedocs server
    if read_the_docs_build:
        # Generate the Doxygen XML files
        subprocess.call('mkdir build; mkdir build/doxygen; cd source; doxygen Doxyfile', shell=True)

    breathe_projects = { "AFF3CT": "../build/doxygen/xml/" }
    breathe_default_project = "AFF3CT"

# -- Configure Exhale (Require the previous Breathe config)

if buildername != "latex":

    # Setup the exhale extension
    exhale_args = {
        # These arguments are required
        "containmentFolder":     "./api",
        "rootFileName":          "library_root.rst",
        "rootFileTitle":         "Library API",
        "doxygenStripFromPath":  "../../include",
        # Suggested optional arguments
        "createTreeView":        True,
        # TIP: if using the sphinx-bootstrap-theme, you need
        # "treeViewIsBootstrap": True,
        "exhaleExecutesDoxygen": True,
        # "verboseBuild":          True,
        "exhaleUseDoxyfile":     True,
        # "exhaleDoxygenStdin": textwrap.dedent('''
        #     INPUT      = ../../include
        #     # Using `=` instead of `+=` overrides
        #     PREDEFINED = DOXYGEN_SHOULD_SKIP_THIS="1"
        # ''')
    }

    # Tell sphinx what the primary language being documented is.
    primary_domain = 'cpp'

    # Tell sphinx what the pygments highlight language should be.
    highlight_language = 'cpp'